#include "wayshadow/window.hpp"

#include <utility>

#include "wayshadow/draw.hpp"

namespace wayshadow {

namespace {

void OneXdgSurfaceConfigure(void* data, struct xdg_surface* surface,
                            uint32_t serial) {
  auto* self = static_cast<WindowManager*>(data);
  xdg_surface_ack_configure(surface, serial);
  if (self && self->State() && self->State()->window_visible) {
    self->Redraw(*self->State());
  }
}

const struct xdg_surface_listener kSurfaceListener = {
  .configure = OneXdgSurfaceConfigure,
};

void OnXdgToplevelConfigure(void* /*data*/, struct xdg_toplevel* /*toplevel*/,
                            int32_t /*w*/, int32_t /*h*/,
                            struct wl_array* /*states*/
) {}

void OnXdgToplevelClose(void* data, struct xdg_toplevel* /*toplevel*/) {
  auto* self = static_cast<WindowManager*>(data);
  if (self && self->State()) {
    self->State()->running = false;
  }
}

void OnXdgToplevelConfigureBounds(void* /*data*/,
                                  struct xdg_toplevel* /*toplevel*/,
                                  int32_t /*w*/, int32_t /*h*/) {}

void OnXdgToplevelWmCapabilities(void* /*data*/,
                                 struct xdg_toplevel* /*toplevel*/,
                                 struct wl_array* /*capabilities*/
) {}

const struct xdg_toplevel_listener kToplevelListener = {
  .configure = OnXdgToplevelConfigure,
  .close = OnXdgToplevelClose,
  .configure_bounds = OnXdgToplevelConfigureBounds,
  .wm_capabilities = OnXdgToplevelWmCapabilities,
};

}  // namespace

WindowManager::WindowManager(WaylandContext& wl_ctx) : wl_ctx_(&wl_ctx) {}

WindowManager::~WindowManager() {
  DestroySurfaces();
  if (buffer_) {
    wl_buffer_destroy(buffer_);
    buffer_ = nullptr;
  }
}

WindowManager::WindowManager(WindowManager&& other) noexcept
    : wl_ctx_(std::exchange(other.wl_ctx_, nullptr)),
      surface_(std::exchange(other.surface_, nullptr)),
      xdg_surface_(std::exchange(other.xdg_surface_, nullptr)),
      xdg_toplevel_(std::exchange(other.xdg_toplevel_, nullptr)),
      buffer_(std::exchange(other.buffer_, nullptr)),
      state_(std::exchange(other.state_, nullptr)) {}

WindowManager& WindowManager::operator=(WindowManager&& other) noexcept {
  if (this != &other) {
    DestroySurfaces();
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

void WindowManager::DestroySurfaces() {
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

void WindowManager::CreateWindow(ClientState& state) {
  state_ = &state;
  if (!wl_ctx_->Compositor() || !wl_ctx_->XdgWmBase()) return;

  surface_ = wl_compositor_create_surface(wl_ctx_->Compositor());
  xdg_surface_ = xdg_wm_base_get_xdg_surface(wl_ctx_->XdgWmBase(), surface_);
  xdg_surface_add_listener(xdg_surface_, &kSurfaceListener, this);

  xdg_toplevel_ = xdg_surface_get_toplevel(xdg_surface_);
  xdg_toplevel_add_listener(xdg_toplevel_, &kToplevelListener, this);
  xdg_toplevel_set_app_id(xdg_toplevel_, "wayshadow");
  xdg_toplevel_set_title(xdg_toplevel_, "WayShadow");

  wl_surface_commit(surface_);
}

void WindowManager::HideWindow(ClientState& state) {
  if (!state.window_visible) return;
  state.window_visible = false;
  state.buffer.Clear();
  state.mouse.last_button.clear();
  state.mouse.lmb = false;
  state.mouse.rmb = false;
  state.mouse.mmb = false;
  state.mouse.back = false;
  state.mouse.forward = false;
  state.mouse.last_lmb = false;
  state.mouse.last_rmb = false;
  state.mouse.last_mmb = false;
  state.mouse.last_back = false;
  state.mouse.last_forward = false;
  state.mouse.has_click = false;
  state.mouse.click_count = 0;
  state.mouse.last_button_id = 0;

  if (surface_) {
    wl_surface_attach(surface_, nullptr, 0, 0);
    wl_surface_commit(surface_);
  }
}

void WindowManager::ShowWindow(ClientState& state) {
  if (state.window_visible) return;

  DestroySurfaces();
  CreateWindow(state);
  wl_display_roundtrip(wl_ctx_->Display());

  state.window_visible = true;
  Redraw(state);
}

void WindowManager::Redraw(ClientState& state) {
  if (!surface_ || !wl_ctx_->Shm()) return;
  Renderer::Redraw(state, surface_, wl_ctx_->Shm(), &buffer_);
}

}  // namespace wayshadow
