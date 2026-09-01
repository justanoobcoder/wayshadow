#pragma once

#include "wayshadow/state.hpp"
#include "wayshadow/wl_setup.hpp"
#include "xdg-shell-client-protocol.h"

#include <wayland-client.h>

namespace wayshadow {

    class WindowManager {
      public:
        explicit WindowManager(WaylandContext& wl_ctx);
        ~WindowManager();

        WindowManager(const WindowManager&) = delete;
        WindowManager& operator=(const WindowManager&) = delete;
        WindowManager(WindowManager&&) noexcept;
        WindowManager& operator=(WindowManager&&) noexcept;

        void create_window(ClientState& state);
        void show_window(ClientState& state);
        void hide_window(ClientState& state);
        void redraw(ClientState& state);

        [[nodiscard]] ClientState* state() const noexcept { return state_; }

      private:
        void destroy_surfaces();

        WaylandContext* wl_ctx_{nullptr};
        struct wl_surface* surface_{nullptr};
        struct xdg_surface* xdg_surface_{nullptr};
        struct xdg_toplevel* xdg_toplevel_{nullptr};
        struct wl_buffer* buffer_{nullptr};
        ClientState* state_{nullptr};
    };

} // namespace wayshadow
