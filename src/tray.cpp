#include "wayshadow/tray.hpp"

#include <cstdio>
#include <filesystem>
#include <unistd.h>

namespace wayshadow {

    TrayIcon::TrayIcon(ClientState& state, WindowManager& win_mgr, GMainLoop* loop)
        : state_(&state), win_mgr_(&win_mgr), loop_(loop) {}

    void TrayIcon::update_toggle_label() {
        if (!toggle_item_ || !state_)
            return;
        const char* label = state_->overlay_enabled ? "\xe2\x98\x91 Show && hide" : "\xe2\x96\xa1 Show && hide";
        gtk_menu_item_set_label(GTK_MENU_ITEM(toggle_item_), label);
    }

    void TrayIcon::on_toggle_activate(GtkMenuItem* /*item*/, void* data) {
        auto* self = static_cast<TrayIcon*>(data);
        if (!self || !self->state_ || !self->win_mgr_)
            return;

        self->state_->overlay_enabled = !self->state_->overlay_enabled;
        self->update_toggle_label();

        if (!self->state_->overlay_enabled) {
            self->win_mgr_->hide_window(*self->state_);
        }
    }

    void TrayIcon::on_exit_activate(GtkMenuItem* /*item*/, void* data) {
        auto* self = static_cast<TrayIcon*>(data);
        if (!self || !self->state_)
            return;

        self->state_->running = false;
        if (self->loop_) {
            g_main_loop_quit(self->loop_);
        }
    }

    bool TrayIcon::init() {
        int argc = 0;
        char** argv = nullptr;
        if (!gtk_init_check(&argc, &argv)) {
            std::fprintf(stderr, "Failed to initialize GTK for tray icon\n");
            return false;
        }

        indicator_ = app_indicator_new("wayshadow-tray", "input-keyboard", APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
        app_indicator_set_status(indicator_, APP_INDICATOR_STATUS_ACTIVE);

        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd))) {
            const auto public_dir = std::filesystem::path(cwd) / "public";
            if (std::filesystem::exists(public_dir)) {
                app_indicator_set_icon_theme_path(indicator_, public_dir.c_str());
                app_indicator_set_icon(indicator_, "key_pop");
            } else {
                const auto parent_public = std::filesystem::path(cwd).parent_path() / "public";
                if (std::filesystem::exists(parent_public)) {
                    app_indicator_set_icon_theme_path(indicator_, parent_public.c_str());
                    app_indicator_set_icon(indicator_, "key_pop");
                }
            }
        }

        menu_ = gtk_menu_new();

        toggle_item_ = gtk_menu_item_new_with_label("\xe2\x98\x91 Show && hide");
        g_signal_connect(toggle_item_, "activate", G_CALLBACK(on_toggle_activate), this);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu_), toggle_item_);

        GtkWidget* quit_item = gtk_menu_item_new_with_label("Exit");
        g_signal_connect(quit_item, "activate", G_CALLBACK(on_exit_activate), this);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu_), quit_item);

        gtk_widget_show_all(menu_);
        app_indicator_set_menu(indicator_, GTK_MENU(menu_));

        return true;
    }

} // namespace wayshadow
