#ifndef WAYSHADOW_XKB_HANDLER_HPP
#define WAYSHADOW_XKB_HANDLER_HPP

#include <xkbcommon/xkbcommon.h>

#include <cstdint>

#include "wayshadow/state.hpp"

namespace wayshadow {

class XkbHandler {
 public:
  XkbHandler();
  ~XkbHandler();

  XkbHandler(const XkbHandler&) = delete;
  XkbHandler& operator=(const XkbHandler&) = delete;
  XkbHandler(XkbHandler&&) noexcept;
  XkbHandler& operator=(XkbHandler&&) noexcept;

  [[nodiscard]] bool IsValid() const noexcept {
    return ctx_ != nullptr && map_ != nullptr && state_ != nullptr;
  }

  void HandleKeyEvent(ClientState& state, uint32_t key, uint32_t state_val);
  void ProcessRepeatKey(ClientState& state, uint32_t key);

 private:
  void ProcessKeyDown(ClientState& state, uint32_t key);
  [[nodiscard]] static const char* GetKeySymbol(xkb_keysym_t keysym) noexcept;
  [[nodiscard]] static bool IsModifier(xkb_keysym_t key) noexcept;

  struct xkb_context* ctx_{nullptr};
  struct xkb_keymap* map_{nullptr};
  struct xkb_state* state_{nullptr};
};

}  // namespace wayshadow

#endif
