#include "wayshadow/wl_setup.hpp"

#include <cstring>
#include <unistd.h>
#include <utility>

namespace wayshadow {

    WaylandContext::~WaylandContext() { disconnect(); }

    WaylandContext::WaylandContext(WaylandContext&& other) noexcept
        : display_(std::exchange(other.display_, nullptr)), registry_(std::exchange(other.registry_, nullptr)),
          compositor_(std::exchange(other.compositor_, nullptr)), shm_(std::exchange(other.shm_, nullptr)),
          xdg_wm_base_(std::exchange(other.xdg_wm_base_, nullptr)), seat_(std::exchange(other.seat_, nullptr)),
          keyboard_(std::exchange(other.keyboard_, nullptr)), state_(std::exchange(other.state_, nullptr)) {}

    WaylandContext& WaylandContext::operator=(WaylandContext&& other) noexcept {
        if (this != &other) {
            disconnect();
            display_ = std::exchange(other.display_, nullptr);
            registry_ = std::exchange(other.registry_, nullptr);
            compositor_ = std::exchange(other.compositor_, nullptr);
            shm_ = std::exchange(other.shm_, nullptr);
            xdg_wm_base_ = std::exchange(other.xdg_wm_base_, nullptr);
            seat_ = std::exchange(other.seat_, nullptr);
            keyboard_ = std::exchange(other.keyboard_, nullptr);
            state_ = std::exchange(other.state_, nullptr);
        }
        return *this;
    }

    void WaylandContext::disconnect() {
        if (keyboard_) {
            wl_keyboard_destroy(keyboard_);
            keyboard_ = nullptr;
        }
        if (seat_) {
            wl_seat_destroy(seat_);
            seat_ = nullptr;
        }
        if (xdg_wm_base_) {
            xdg_wm_base_destroy(xdg_wm_base_);
            xdg_wm_base_ = nullptr;
        }
        if (shm_) {
            wl_shm_destroy(shm_);
            shm_ = nullptr;
        }
        if (compositor_) {
            wl_compositor_destroy(compositor_);
            compositor_ = nullptr;
        }
        if (registry_) {
            wl_registry_destroy(registry_);
            registry_ = nullptr;
        }
        if (display_) {
            wl_display_disconnect(display_);
            display_ = nullptr;
        }
    }

    static void on_keyboard_keymap(
        void* /*data*/, struct wl_keyboard* /*wl_keyboard*/, uint32_t /*format*/, int32_t fd, uint32_t /*size*/
    ) {
        close(fd);
    }

    static void on_keyboard_enter(
        void* /*data*/, struct wl_keyboard* /*wl_keyboard*/, uint32_t /*serial*/, struct wl_surface* /*surface*/,
        struct wl_array* /*keys*/
    ) {}
    static void on_keyboard_leave(
        void* /*data*/, struct wl_keyboard* /*wl_keyboard*/, uint32_t /*serial*/, struct wl_surface* /*surface*/
    ) {}
    static void on_keyboard_key(
        void* /*data*/, struct wl_keyboard* /*wl_keyboard*/, uint32_t /*serial*/, uint32_t /*time*/, uint32_t /*key*/,
        uint32_t /*state*/
    ) {}
    static void on_keyboard_modifiers(
        void* /*data*/, struct wl_keyboard* /*wl_keyboard*/, uint32_t /*serial*/, uint32_t /*mods_depressed*/,
        uint32_t /*mods_latched*/, uint32_t /*mods_locked*/, uint32_t /*group*/
    ) {}

    static void on_keyboard_repeat_info(void* data, struct wl_keyboard* /*wl_keyboard*/, int32_t rate, int32_t delay) {
        auto* ctx = static_cast<WaylandContext*>(data);
        if (ctx && ctx->state()) {
            ctx->state()->repeat_rate = rate;
            ctx->state()->repeat_delay = delay;
        }
    }

    static const struct wl_keyboard_listener keyboard_listener = {
        .keymap = on_keyboard_keymap,
        .enter = on_keyboard_enter,
        .leave = on_keyboard_leave,
        .key = on_keyboard_key,
        .modifiers = on_keyboard_modifiers,
        .repeat_info = on_keyboard_repeat_info,
    };

    static void on_seat_capabilities(void* data, struct wl_seat* seat, uint32_t capabilities) {
        auto* ctx = static_cast<WaylandContext*>(data);
        if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD) {
            ctx->set_keyboard(wl_seat_get_keyboard(seat));
            wl_keyboard_add_listener(ctx->keyboard(), &keyboard_listener, ctx);
        }
    }

    static void on_seat_name(void* /*data*/, struct wl_seat* /*seat*/, const char* /*name*/) {}

    static const struct wl_seat_listener seat_listener = {
        .capabilities = on_seat_capabilities,
        .name = on_seat_name,
    };

    static void on_xdg_wm_base_ping(void* /*data*/, struct xdg_wm_base* base, uint32_t serial) {
        xdg_wm_base_pong(base, serial);
    }

    static const struct xdg_wm_base_listener xdg_wm_base_listener = {
        .ping = on_xdg_wm_base_ping,
    };

    static void
    on_registry_global(void* data, struct wl_registry* reg, uint32_t name, const char* iface, uint32_t /*version*/) {
        auto* ctx = static_cast<WaylandContext*>(data);
        if (std::strcmp(iface, wl_compositor_interface.name) == 0) {
            ctx->set_compositor(
                static_cast<struct wl_compositor*>(wl_registry_bind(reg, name, &wl_compositor_interface, 4))
            );
        } else if (std::strcmp(iface, wl_shm_interface.name) == 0) {
            ctx->set_shm(static_cast<struct wl_shm*>(wl_registry_bind(reg, name, &wl_shm_interface, 1)));
        } else if (std::strcmp(iface, xdg_wm_base_interface.name) == 0) {
            auto* base = static_cast<struct xdg_wm_base*>(wl_registry_bind(reg, name, &xdg_wm_base_interface, 1));
            ctx->set_xdg_wm_base(base);
            xdg_wm_base_add_listener(base, &xdg_wm_base_listener, ctx);
        } else if (std::strcmp(iface, wl_seat_interface.name) == 0) {
            auto* seat = static_cast<struct wl_seat*>(wl_registry_bind(reg, name, &wl_seat_interface, 5));
            ctx->set_seat(seat);
            wl_seat_add_listener(seat, &seat_listener, ctx);
        }
    }

    static void on_registry_global_remove(void* /*data*/, struct wl_registry* /*reg*/, uint32_t /*name*/) {}

    static const struct wl_registry_listener registry_listener = {
        .global = on_registry_global,
        .global_remove = on_registry_global_remove,
    };

    bool WaylandContext::connect(ClientState& state) {
        state_ = &state;
        display_ = wl_display_connect(nullptr);
        if (!display_)
            return false;

        registry_ = wl_display_get_registry(display_);
        wl_registry_add_listener(registry_, &registry_listener, this);
        wl_display_roundtrip(display_);

        return (compositor_ && shm_ && xdg_wm_base_);
    }

} // namespace wayshadow
