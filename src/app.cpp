#include "keypop/app.hpp"

#include <cerrno>
#include <cstdio>
#include <ctime>

namespace keypop {

    namespace {
        long time_diff_ms(const struct timespec* start, const struct timespec* end) {
            return (end->tv_sec - start->tv_sec) * 1000 + (end->tv_nsec - start->tv_nsec) / 1000000;
        }
    } // namespace

    Application::Application(Config config) {
        state_.config = config;
        clock_gettime(CLOCK_MONOTONIC, &state_.last_key_time);
    }

    Application::~Application() {
        if (state_.repeat_timer_id) {
            g_source_remove(state_.repeat_timer_id);
            state_.repeat_timer_id = 0;
        }
        if (loop_) {
            g_main_loop_unref(loop_);
            loop_ = nullptr;
        }
    }

    gboolean Application::on_wayland_event(GIOChannel* /*source*/, GIOCondition condition, gpointer data) {
        auto* app = static_cast<Application*>(data);
        if (condition & G_IO_IN) {
            if (wl_display_dispatch(app->wl_ctx_.display()) == -1) {
                return FALSE;
            }
        }
        if (condition & (G_IO_ERR | G_IO_HUP)) {
            return FALSE;
        }
        return TRUE;
    }

    gboolean Application::on_input_event(GIOChannel* /*source*/, GIOCondition condition, gpointer data) {
        auto* app = static_cast<Application*>(data);
        if (condition & G_IO_IN) {
            if (app->input_) {
                app->input_->dispatch();
            }
        }
        return TRUE;
    }

    gboolean Application::on_repeat_rate_tick(gpointer data) {
        auto* app = static_cast<Application*>(data);
        app->xkb_.process_repeat_key(app->state_, app->state_.repeat_key);
        if (app->state_.needs_redraw && app->win_mgr_) {
            app->win_mgr_->redraw(app->state_);
            app->state_.needs_redraw = false;
        }
        return TRUE;
    }

    gboolean Application::on_repeat_delay_done(gpointer data) {
        auto* app = static_cast<Application*>(data);
        app->xkb_.process_repeat_key(app->state_, app->state_.repeat_key);
        if (app->state_.needs_redraw && app->win_mgr_) {
            app->win_mgr_->redraw(app->state_);
            app->state_.needs_redraw = false;
        }

        if (app->state_.repeat_rate > 0) {
            app->state_.repeat_timer_id =
                g_timeout_add(static_cast<guint>(1000 / app->state_.repeat_rate), on_repeat_rate_tick, app);
        } else {
            app->state_.repeat_timer_id = 0;
        }
        return FALSE;
    }

    void Application::setup_repeat_timer() {
        if (state_.repeat_timer_id) {
            g_source_remove(state_.repeat_timer_id);
            state_.repeat_timer_id = 0;
        }

        if (state_.repeat_key != 0 && state_.repeat_rate > 0 && state_.repeat_delay > 0) {
            state_.repeat_timer_id = g_timeout_add(static_cast<guint>(state_.repeat_delay), on_repeat_delay_done, this);
        }
    }

    gboolean Application::on_timer_tick(gpointer data) {
        auto* app = static_cast<Application*>(data);
        auto& state = app->state_;

        // Handle auto-hide duration
        if (state.window_visible) {
            struct timespec now{0, 0};
            clock_gettime(CLOCK_MONOTONIC, &now);
            if (state.config.hide_timeout_ms > 0
                && time_diff_ms(&state.last_key_time, &now) > state.config.hide_timeout_ms) {
                if (app->win_mgr_) {
                    app->win_mgr_->hide_window(state);
                }
                state.needs_redraw = false;
            }
        }

        // Refresh overlay
        if (state.needs_redraw && state.window_visible && app->win_mgr_) {
            app->win_mgr_->redraw(state);
            state.needs_redraw = false;
        }

        if (wl_display_flush(app->wl_ctx_.display()) < 0 && errno != EAGAIN) {
            return FALSE;
        }

        return TRUE;
    }

    int Application::run() {
        if (!wl_ctx_.connect(state_)) {
            std::fprintf(stderr, "Failed to connect to Wayland\n");
            return 1;
        }

        if (!xkb_.is_valid()) {
            std::fprintf(stderr, "Failed to initialize XKB context\n");
            return 1;
        }

        win_mgr_ = std::make_unique<WindowManager>(wl_ctx_);
        win_mgr_->create_window(state_);

        input_ = std::make_unique<InputManager>(
            [this](uint32_t key, uint32_t key_state) {
                if (!state_.window_visible && state_.overlay_enabled && key_state == 1) {
                    if (win_mgr_) {
                        win_mgr_->show_window(state_);
                    }
                }

                const uint32_t prev_repeat_key = state_.repeat_key;
                xkb_.handle_key_event(state_, key, key_state);

                if (state_.repeat_key != prev_repeat_key) {
                    setup_repeat_timer();
                }

                if (state_.needs_redraw && win_mgr_) {
                    win_mgr_->redraw(state_);
                    state_.needs_redraw = false;
                }
            },
            state_
        );

        if (!input_->is_valid()) {
            std::fprintf(stderr, "Warning: Failed to initialize libinput context\n");
        }

        loop_ = g_main_loop_new(nullptr, FALSE);

        tray_ = std::make_unique<TrayIcon>(state_, *win_mgr_, loop_);
        if (!tray_->init()) {
            std::fprintf(stderr, "Warning: Failed to initialize tray icon\n");
        }

        GIOChannel* wl_chan = g_io_channel_unix_new(wl_display_get_fd(wl_ctx_.display()));
        g_io_add_watch(wl_chan, static_cast<GIOCondition>(G_IO_IN | G_IO_ERR | G_IO_HUP), on_wayland_event, this);
        g_io_channel_unref(wl_chan);

        if (input_->is_valid()) {
            GIOChannel* in_chan = g_io_channel_unix_new(input_->get_fd());
            g_io_add_watch(in_chan, G_IO_IN, on_input_event, this);
            g_io_channel_unref(in_chan);
        }

        g_timeout_add(16, on_timer_tick, this);

        wl_display_roundtrip(wl_ctx_.display());

        g_main_loop_run(loop_);

        return 0;
    }

} // namespace keypop
