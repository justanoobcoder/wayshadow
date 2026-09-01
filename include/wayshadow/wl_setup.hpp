#pragma once

#include "wayshadow/state.hpp"
#include "xdg-shell-client-protocol.h"

#include <wayland-client.h>

namespace wayshadow {

    class WaylandContext {
      public:
        WaylandContext() = default;
        ~WaylandContext();

        WaylandContext(const WaylandContext&) = delete;
        WaylandContext& operator=(const WaylandContext&) = delete;
        WaylandContext(WaylandContext&&) noexcept;
        WaylandContext& operator=(WaylandContext&&) noexcept;

        [[nodiscard]] bool connect(ClientState& state);
        void disconnect();

        [[nodiscard]] struct wl_display* display() const noexcept { return display_; }
        [[nodiscard]] struct wl_compositor* compositor() const noexcept { return compositor_; }
        [[nodiscard]] struct wl_shm* shm() const noexcept { return shm_; }
        [[nodiscard]] struct xdg_wm_base* xdg_wm_base() const noexcept { return xdg_wm_base_; }
        [[nodiscard]] struct wl_keyboard* keyboard() const noexcept { return keyboard_; }
        [[nodiscard]] ClientState* state() const noexcept { return state_; }

        void set_compositor(struct wl_compositor* c) noexcept { compositor_ = c; }
        void set_shm(struct wl_shm* s) noexcept { shm_ = s; }
        void set_xdg_wm_base(struct xdg_wm_base* base) noexcept { xdg_wm_base_ = base; }
        void set_seat(struct wl_seat* s) noexcept { seat_ = s; }
        void set_keyboard(struct wl_keyboard* k) noexcept { keyboard_ = k; }

      private:
        struct wl_display* display_{nullptr};
        struct wl_registry* registry_{nullptr};
        struct wl_compositor* compositor_{nullptr};
        struct wl_shm* shm_{nullptr};
        struct xdg_wm_base* xdg_wm_base_{nullptr};
        struct wl_seat* seat_{nullptr};
        struct wl_keyboard* keyboard_{nullptr};
        ClientState* state_{nullptr};
    };

} // namespace wayshadow
