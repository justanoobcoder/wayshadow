#pragma once

#include "keypop/config.hpp"
#include "keypop/input.hpp"
#include "keypop/state.hpp"
#include "keypop/tray.hpp"
#include "keypop/window.hpp"
#include "keypop/wl_setup.hpp"
#include "keypop/xkb_handler.hpp"

#include <glib.h>

namespace keypop {

    class Application {
      public:
        explicit Application(Config config);
        ~Application();

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        Application(Application&&) noexcept = default;
        Application& operator=(Application&&) noexcept = default;

        int run();

      private:
        static gboolean on_wayland_event(GIOChannel* source, GIOCondition condition, gpointer data);
        static gboolean on_input_event(GIOChannel* source, GIOCondition condition, gpointer data);
        static gboolean on_timer_tick(gpointer data);
        static gboolean on_repeat_delay_done(gpointer data);
        static gboolean on_repeat_rate_tick(gpointer data);

        void setup_repeat_timer();

        ClientState state_{};
        WaylandContext wl_ctx_{};
        XkbHandler xkb_{};
        std::unique_ptr<InputManager> input_{nullptr};
        std::unique_ptr<WindowManager> win_mgr_{nullptr};
        std::unique_ptr<TrayIcon> tray_{nullptr};
        GMainLoop* loop_{nullptr};
    };

} // namespace keypop
