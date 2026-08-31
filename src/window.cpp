#include "keypop/window.hpp"

#include "keypop/draw.hpp"

#include <utility>

namespace keypop {

    WindowManager::WindowManager(WaylandContext& wl_ctx) : wl_ctx_(&wl_ctx) {}

    WindowManager::~WindowManager() {
        destroy_surfaces();
        if (buffer_) {
            wl_buffer_destroy(buffer_);
            buffer_ = nullptr;
        }
    }

    WindowManager::WindowManager(WindowManager&& other) noexcept
        : wl_ctx_(std::exchange(other.wl_ctx_, nullptr)), surface_(std::exchange(other.surface_, nullptr)),
          xdg_surface_(std::exchange(other.xdg_surface_, nullptr)),
          xdg_toplevel_(std::exchange(other.xdg_toplevel_, nullptr)), buffer_(std::exchange(other.buffer_, nullptr)),
          state_(std::exchange(other.state_, nullptr)) {}

    WindowManager& WindowManager::operator=(WindowManager&& other) noexcept {
        if (this != &other) {
            destroy_surfaces();
            if (buffer_) {
                wl_buffer_destroy(buffer_);
            }

            wl_ctx_ = std::exchange(other.wl_ctx_, nullptr);
            surface_ = std::exchange(other.surface_, nullptr);
            xdg_surface_ = std::exchange(other.xdg_surface_, nullptr);
            xdg_toplevel_ = std::exchange(other.xdg_toplevel_, nullptr);
            buffer_ = std::exchange(other.buffer_, nullptr);
            state_ = std::exchange(other.state_, nullptr);
        }
        return *this;
    }

    void WindowManager::destroy_surfaces() {
        if (xdg_toplevel_) {
            xdg_toplevel_destroy(xdg_toplevel_);
            xdg_toplevel_ = nullptr;
        }
        if (xdg_surface_) {
            xdg_surface_destroy(xdg_surface_);
            xdg_surface_ = nullptr;
        }
        if (surface_) {
            wl_surface_destroy(surface_);
            surface_ = nullptr;
        }
    }

    static void on_xdg_surface_configure(void* data, struct xdg_surface* surface, uint32_t serial) {
        auto* self = static_cast<WindowManager*>(data);
        xdg_surface_ack_configure(surface, serial);
        if (self && self->state() && self->state()->window_visible) {
            self->redraw(*self->state());
        }
    }

    static const struct xdg_surface_listener surface_listener = {
        .configure = on_xdg_surface_configure,
    };

    static void on_xdg_toplevel_configure(
        void* /*data*/, struct xdg_toplevel* /*toplevel*/, int32_t /*w*/, int32_t /*h*/, struct wl_array* /*states*/
    ) {}

    static void on_xdg_toplevel_close(void* data, struct xdg_toplevel* /*toplevel*/) {
        auto* self = static_cast<WindowManager*>(data);
        if (self && self->state()) {
            self->state()->running = false;
        }
    }

    static void
    on_xdg_toplevel_configure_bounds(void* /*data*/, struct xdg_toplevel* /*toplevel*/, int32_t /*w*/, int32_t /*h*/) {}

    static void on_xdg_toplevel_wm_capabilities(
        void* /*data*/, struct xdg_toplevel* /*toplevel*/, struct wl_array* /*capabilities*/
    ) {}

    static const struct xdg_toplevel_listener toplevel_listener = {
        .configure = on_xdg_toplevel_configure,
        .close = on_xdg_toplevel_close,
        .configure_bounds = on_xdg_toplevel_configure_bounds,
        .wm_capabilities = on_xdg_toplevel_wm_capabilities,
    };

    void WindowManager::create_window(ClientState& state) {
        state_ = &state;
        if (!wl_ctx_->compositor() || !wl_ctx_->xdg_wm_base())
            return;

        surface_ = wl_compositor_create_surface(wl_ctx_->compositor());
        xdg_surface_ = xdg_wm_base_get_xdg_surface(wl_ctx_->xdg_wm_base(), surface_);
        xdg_surface_add_listener(xdg_surface_, &surface_listener, this);

        xdg_toplevel_ = xdg_surface_get_toplevel(xdg_surface_);
        xdg_toplevel_add_listener(xdg_toplevel_, &toplevel_listener, this);
        xdg_toplevel_set_app_id(xdg_toplevel_, "keypop");
        xdg_toplevel_set_title(xdg_toplevel_, "Show Me The Key");

        wl_surface_commit(surface_);
    }

    void WindowManager::hide_window(ClientState& state) {
        if (!state.window_visible)
            return;
        state.window_visible = false;
        state.buffer.clear();

        if (surface_) {
            wl_surface_attach(surface_, nullptr, 0, 0);
            wl_surface_commit(surface_);
        }
    }

    void WindowManager::show_window(ClientState& state) {
        if (state.window_visible)
            return;

        destroy_surfaces();
        create_window(state);
        wl_display_roundtrip(wl_ctx_->display());

        state.window_visible = true;
        redraw(state);
    }

    void WindowManager::redraw(ClientState& state) {
        if (!surface_ || !wl_ctx_->shm())
            return;
        Renderer::redraw(state, surface_, wl_ctx_->shm(), &buffer_);
    }

} // namespace keypop
