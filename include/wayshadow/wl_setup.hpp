#ifndef WAYSHADOW_WL_SETUP_HPP
#define WAYSHADOW_WL_SETUP_HPP

#include <wayland-client.h>

#include "wayshadow/state.hpp"
#include "xdg-shell-client-protocol.h"

namespace wayshadow {

class WaylandContext {
 public:
  WaylandContext() = default;
  ~WaylandContext();

  WaylandContext(const WaylandContext&) = delete;
  WaylandContext& operator=(const WaylandContext&) = delete;
  WaylandContext(WaylandContext&&) noexcept;
  WaylandContext& operator=(WaylandContext&&) noexcept;

  [[nodiscard]] bool Connect(ClientState& state);
  void Disconnect();

  [[nodiscard]] struct wl_display* Display() const noexcept { return display_; }
  [[nodiscard]] struct wl_compositor* Compositor() const noexcept {
    return compositor_;
  }
  [[nodiscard]] struct wl_shm* Shm() const noexcept { return shm_; }
  [[nodiscard]] struct xdg_wm_base* XdgWmBase() const noexcept {
    return xdg_wm_base_;
  }
  [[nodiscard]] struct wl_keyboard* Keyboard() const noexcept {
    return keyboard_;
  }
  [[nodiscard]] ClientState* State() const noexcept { return state_; }

  void SetCompositor(struct wl_compositor* c) noexcept { compositor_ = c; }
  void SetShm(struct wl_shm* s) noexcept { shm_ = s; }
  void SetXdgWmBase(struct xdg_wm_base* base) noexcept { xdg_wm_base_ = base; }
  void SetSeat(struct wl_seat* s) noexcept { seat_ = s; }
  void SetKeyboard(struct wl_keyboard* k) noexcept { keyboard_ = k; }

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

}  // namespace wayshadow

#endif
