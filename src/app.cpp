#include "wayshadow/app.hpp"

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <ctime>

namespace wayshadow {

namespace {

int64_t TimeDiffMs(const struct timespec* start, const struct timespec* end) {
  return ((end->tv_sec - start->tv_sec) * 1000) +
         ((end->tv_nsec - start->tv_nsec) / 1000000);
}

}  // namespace

Application::Application(Config config) {
  state_.config = config;
  clock_gettime(CLOCK_MONOTONIC, &state_.last_key_time);
}

Application::~Application() {
  if (state_.repeat_timer_id) {
    g_source_remove(state_.repeat_timer_id);
    state_.repeat_timer_id = 0;
  }
  if (hold_timer_id_) {
    g_source_remove(hold_timer_id_);
    hold_timer_id_ = 0;
  }
  if (loop_) {
    g_main_loop_unref(loop_);
    loop_ = nullptr;
  }
}

gboolean Application::OnWaylandEvent(GIOChannel* /*source*/,
                                     GIOCondition condition, gpointer data) {
  auto* app = static_cast<Application*>(data);
  if (condition & G_IO_IN) {
    if (wl_display_dispatch(app->wl_ctx_.Display()) == -1) {
      return FALSE;
    }
  }
  if (condition & (G_IO_ERR | G_IO_HUP)) {
    return FALSE;
  }
  return TRUE;
}

gboolean Application::OnInputEvent(GIOChannel* /*source*/,
                                   GIOCondition condition, gpointer data) {
  auto* app = static_cast<Application*>(data);
  if (condition & G_IO_IN) {
    if (app->input_) {
      app->input_->Dispatch();
    }
  }
  return TRUE;
}

gboolean Application::OnRepeatRateTick(gpointer data) {
  auto* app = static_cast<Application*>(data);
  app->xkb_.ProcessRepeatKey(app->state_, app->state_.repeat_key);
  if (app->state_.needs_redraw && app->win_mgr_) {
    app->win_mgr_->Redraw(app->state_);
    app->state_.needs_redraw = false;
  }
  return TRUE;
}

gboolean Application::OnRepeatDelayDone(gpointer data) {
  auto* app = static_cast<Application*>(data);
  app->xkb_.ProcessRepeatKey(app->state_, app->state_.repeat_key);
  if (app->state_.needs_redraw && app->win_mgr_) {
    app->win_mgr_->Redraw(app->state_);
    app->state_.needs_redraw = false;
  }

  if (app->state_.repeat_rate > 0) {
    app->state_.repeat_timer_id =
      g_timeout_add(static_cast<guint>(1000 / app->state_.repeat_rate),
                    OnRepeatRateTick, app);
  } else {
    app->state_.repeat_timer_id = 0;
  }
  return FALSE;
}

void Application::SetupRepeatTimer() {
  if (state_.repeat_timer_id) {
    g_source_remove(state_.repeat_timer_id);
    state_.repeat_timer_id = 0;
  }

  if (state_.repeat_key != 0 && state_.repeat_rate > 0 &&
      state_.repeat_delay > 0) {
    state_.repeat_timer_id = g_timeout_add(
      static_cast<guint>(state_.repeat_delay), OnRepeatDelayDone, this);
  }
}

gboolean Application::OnHoldTimer(gpointer data) {
  auto* app = static_cast<Application*>(data);
  app->hold_timer_id_ = 0;
  // If any button is still held, trigger a redraw so HOLD state becomes visible
  auto& m = app->state_.mouse;
  if ((m.lmb || m.rmb || m.mmb || m.back || m.forward) &&
      app->state_.window_visible) {
    if (app->win_mgr_) {
      app->win_mgr_->Redraw(app->state_);
    }
  }
  return FALSE;  // one-shot
}

gboolean Application::OnTimerTick(gpointer data) {
  auto* app = static_cast<Application*>(data);
  auto& state = app->state_;

  // Handle auto-hide duration
  if (state.window_visible) {
    struct timespec now{.tv_sec = 0, .tv_nsec = 0};
    clock_gettime(CLOCK_MONOTONIC, &now);
    if (state.config.hide_timeout_ms > 0 &&
        TimeDiffMs(&state.last_key_time, &now) > state.config.hide_timeout_ms) {
      if (app->win_mgr_) {
        app->win_mgr_->HideWindow(state);
      }
      state.needs_redraw = false;
    }
  }

  // Refresh overlay
  if (state.needs_redraw && state.window_visible && app->win_mgr_) {
    app->win_mgr_->Redraw(state);
    state.needs_redraw = false;
  }

  if (wl_display_flush(app->wl_ctx_.Display()) < 0 && errno != EAGAIN) {
    return FALSE;
  }

  return TRUE;
}

int Application::Run() {
  if (!wl_ctx_.Connect(state_)) {
    std::fprintf(stderr, "Failed to connect to Wayland\n");
    return 1;
  }

  if (!xkb_.IsValid()) {
    std::fprintf(stderr, "Failed to initialize XKB context\n");
    return 1;
  }

  win_mgr_ = std::make_unique<WindowManager>(wl_ctx_);
  win_mgr_->CreateWindow(state_);

  input_ = std::make_unique<InputManager>(
    [this](uint32_t key, uint32_t key_state) {
      if (!state_.window_visible && state_.overlay_enabled && key_state == 1) {
        if (win_mgr_) {
          win_mgr_->ShowWindow(state_);
        }
      }

      const uint32_t prev_repeat_key = state_.repeat_key;
      xkb_.HandleKeyEvent(state_, key, key_state);

      if (state_.repeat_key != prev_repeat_key) {
        SetupRepeatTimer();
      }

      if (state_.needs_redraw && win_mgr_) {
        win_mgr_->Redraw(state_);
        state_.needs_redraw = false;
      }
    },
    [this](uint32_t /*button*/, uint32_t button_state) {
      if (button_state == 1) {
        if (!state_.window_visible && state_.overlay_enabled) {
          if (win_mgr_) {
            win_mgr_->ShowWindow(state_);
          }
        }
        state_.needs_redraw = true;
        // Schedule a deferred redraw to activate HOLD state after 300ms
        if (hold_timer_id_) {
          g_source_remove(hold_timer_id_);
        }
        hold_timer_id_ = g_timeout_add(300, OnHoldTimer, this);
      } else if (button_state == 0 && state_.window_visible) {
        // Cancel pending hold timer on release
        if (hold_timer_id_) {
          g_source_remove(hold_timer_id_);
          hold_timer_id_ = 0;
        }
        // Redraw on release so HOLD state clears immediately
        state_.needs_redraw = true;
      }

      if (state_.needs_redraw && win_mgr_) {
        win_mgr_->Redraw(state_);
        state_.needs_redraw = false;
      }
    },
    [this](double /*dx*/, double /*dy*/) {
      if ((state_.mouse.lmb || state_.mouse.rmb || state_.mouse.mmb ||
           state_.mouse.back || state_.mouse.forward) &&
          state_.window_visible) {
        clock_gettime(CLOCK_MONOTONIC, &state_.last_key_time);
        if (win_mgr_) {
          win_mgr_->Redraw(state_);
        }
      }
    },
    state_);

  if (!input_->IsValid()) {
    std::fprintf(stderr, "Warning: Failed to initialize libinput context\n");
  }

  loop_ = g_main_loop_new(nullptr, FALSE);

  tray_ = std::make_unique<TrayIcon>(state_, *win_mgr_, loop_);
  if (!tray_->Init()) {
    std::fprintf(stderr, "Warning: Failed to initialize tray icon\n");
  }

  GIOChannel* wl_chan =
    g_io_channel_unix_new(wl_display_get_fd(wl_ctx_.Display()));
  g_io_add_watch(wl_chan,
                 static_cast<GIOCondition>(G_IO_IN | G_IO_ERR | G_IO_HUP),
                 OnWaylandEvent, this);
  g_io_channel_unref(wl_chan);

  if (input_->IsValid()) {
    GIOChannel* in_chan = g_io_channel_unix_new(input_->GetFd());
    g_io_add_watch(in_chan, G_IO_IN, OnInputEvent, this);
    g_io_channel_unref(in_chan);
  }

  g_timeout_add(16, OnTimerTick, this);

  wl_display_roundtrip(wl_ctx_.Display());

  g_main_loop_run(loop_);

  return 0;
}

}  // namespace wayshadow
