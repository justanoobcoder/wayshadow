#include "wayshadow/wl_setup.hpp"

#include <unistd.h>

#include <cstring>
#include <utility>

namespace wayshadow {

WaylandContext::~WaylandContext() { Disconnect(); }

WaylandContext::WaylandContext(WaylandContext&& other) noexcept
    : display_(std::exchange(other.display_, nullptr)),
      registry_(std::exchange(other.registry_, nullptr)),
      compositor_(std::exchange(other.compositor_, nullptr)),
      shm_(std::exchange(other.shm_, nullptr)),
      xdg_wm_base_(std::exchange(other.xdg_wm_base_, nullptr)),
      seat_(std::exchange(other.seat_, nullptr)),
      keyboard_(std::exchange(other.keyboard_, nullptr)),
      state_(std::exchange(other.state_, nullptr)) {}

WaylandContext& WaylandContext::operator=(WaylandContext&& other) noexcept {
  if (this != &other) {
    Disconnect();
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

void WaylandContext::Disconnect() {
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

static void OnKeyboardKeymap(void* /*data*/,
                             struct wl_keyboard* /*wl_keyboard*/,
                             uint32_t /*format*/, int32_t fd, uint32_t /*size*/
) {
  close(fd);
}

static void OnKeyboardEnter(void* /*data*/, struct wl_keyboard* /*wl_keyboard*/,
                            uint32_t /*serial*/, struct wl_surface* /*surface*/,
                            struct wl_array* /*keys*/
) {}
static void OnKeyboardLeave(void* /*data*/, struct wl_keyboard* /*wl_keyboard*/,
                            uint32_t /*serial*/, struct wl_surface* /*surface*/
) {}
static void OnKeyboardKey(void* /*data*/, struct wl_keyboard* /*wl_keyboard*/,
                          uint32_t /*serial*/, uint32_t /*time*/,
                          uint32_t /*key*/, uint32_t /*state*/
) {}
static void OnKeyboardModifiers(void* /*data*/,
                                struct wl_keyboard* /*wl_keyboard*/,
                                uint32_t /*serial*/,
                                uint32_t /*mods_depressed*/,
                                uint32_t /*mods_latched*/,
                                uint32_t /*mods_locked*/, uint32_t /*group*/
) {}

static void OnKeyboardRepeatInfo(void* data,
                                 struct wl_keyboard* /*wl_keyboard*/,
                                 int32_t rate, int32_t delay) {
  auto* ctx = static_cast<WaylandContext*>(data);
  if (ctx && ctx->State()) {
    ctx->State()->repeat_rate = rate;
    ctx->State()->repeat_delay = delay;
  }
}

static const struct wl_keyboard_listener kEyboardListener = {
  .keymap = OnKeyboardKeymap,
  .enter = OnKeyboardEnter,
  .leave = OnKeyboardLeave,
  .key = OnKeyboardKey,
  .modifiers = OnKeyboardModifiers,
  .repeat_info = OnKeyboardRepeatInfo,
};

static void OnSeatCapabilities(void* data, struct wl_seat* seat,
                               uint32_t capabilities) {
  auto* ctx = static_cast<WaylandContext*>(data);
  if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD) {
    ctx->SetKeyboard(wl_seat_get_keyboard(seat));
    wl_keyboard_add_listener(ctx->Keyboard(), &kEyboardListener, ctx);
  }
}

static void OnSeatName(void* /*data*/, struct wl_seat* /*seat*/,
                       const char* /*name*/) {}

static const struct wl_seat_listener kSeatListener = {
  .capabilities = OnSeatCapabilities,
  .name = OnSeatName,
};

static void OnXdgWmBasePing(void* /*data*/, struct xdg_wm_base* base,
                            uint32_t serial) {
  xdg_wm_base_pong(base, serial);
}

static const struct xdg_wm_base_listener kXdgWmBaseListener = {
  .ping = OnXdgWmBasePing,
};

static void OnRegistryGlobal(void* data, struct wl_registry* reg, uint32_t name,
                             const char* iface, uint32_t /*version*/) {
  auto* ctx = static_cast<WaylandContext*>(data);
  if (std::strcmp(iface, wl_compositor_interface.name) == 0) {
    ctx->SetCompositor(static_cast<struct wl_compositor*>(
      wl_registry_bind(reg, name, &wl_compositor_interface, 4)));
  } else if (std::strcmp(iface, wl_shm_interface.name) == 0) {
    ctx->SetShm(static_cast<struct wl_shm*>(
      wl_registry_bind(reg, name, &wl_shm_interface, 1)));
  } else if (std::strcmp(iface, xdg_wm_base_interface.name) == 0) {
    auto* base = static_cast<struct xdg_wm_base*>(
      wl_registry_bind(reg, name, &xdg_wm_base_interface, 1));
    ctx->SetXdgWmBase(base);
    xdg_wm_base_add_listener(base, &kXdgWmBaseListener, ctx);
  } else if (std::strcmp(iface, wl_seat_interface.name) == 0) {
    auto* seat = static_cast<struct wl_seat*>(
      wl_registry_bind(reg, name, &wl_seat_interface, 5));
    ctx->SetSeat(seat);
    wl_seat_add_listener(seat, &kSeatListener, ctx);
  }
}

static void OnRegistryGlobalRemove(void* /*data*/, struct wl_registry* /*reg*/,
                                   uint32_t /*name*/) {}

static const struct wl_registry_listener kRegistryListener = {
  .global = OnRegistryGlobal,
  .global_remove = OnRegistryGlobalRemove,
};

bool WaylandContext::Connect(ClientState& state) {
  state_ = &state;
  display_ = wl_display_connect(nullptr);
  if (!display_) return false;

  registry_ = wl_display_get_registry(display_);
  wl_registry_add_listener(registry_, &kRegistryListener, this);
  wl_display_roundtrip(display_);

  return ((compositor_ != nullptr) && (shm_ != nullptr) &&
          (xdg_wm_base_ != nullptr));
}

}  // namespace wayshadow
