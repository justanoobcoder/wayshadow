#pragma once

#include "wayshadow/state.hpp"
#include "wayshadow/window.hpp"

#include <gtk/gtk.h>
#include <libappindicator/app-indicator.h>

namespace wayshadow {

    class TrayIcon {
      public:
        TrayIcon(ClientState& state, WindowManager& win_mgr, GMainLoop* loop);
        ~TrayIcon() = default;

        TrayIcon(const TrayIcon&) = delete;
        TrayIcon& operator=(const TrayIcon&) = delete;
        TrayIcon(TrayIcon&&) noexcept = default;
        TrayIcon& operator=(TrayIcon&&) noexcept = default;

        [[nodiscard]] bool init();

      private:
        static void on_toggle_activate(GtkMenuItem* item, void* data);
        static void on_exit_activate(GtkMenuItem* item, void* data);
        void update_toggle_label();

        ClientState* state_{nullptr};
        WindowManager* win_mgr_{nullptr};
        GMainLoop* loop_{nullptr};
        AppIndicator* indicator_{nullptr};
        GtkWidget* menu_{nullptr};
        GtkWidget* toggle_item_{nullptr};
    };

} // namespace wayshadow
