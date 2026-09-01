#pragma once

#include "wayshadow/state.hpp"

#include <cstdint>
#include <xkbcommon/xkbcommon.h>

namespace wayshadow {

    class XkbHandler {
      public:
        XkbHandler();
        ~XkbHandler();

        XkbHandler(const XkbHandler&) = delete;
        XkbHandler& operator=(const XkbHandler&) = delete;
        XkbHandler(XkbHandler&&) noexcept;
        XkbHandler& operator=(XkbHandler&&) noexcept;

        [[nodiscard]] bool is_valid() const noexcept { return ctx_ != nullptr && map_ != nullptr && state_ != nullptr; }

        void handle_key_event(ClientState& state, uint32_t key, uint32_t state_val);
        void process_repeat_key(ClientState& state, uint32_t key);

      private:
        void process_key_down(ClientState& state, uint32_t key);
        [[nodiscard]] static const char* get_key_symbol(xkb_keysym_t keysym) noexcept;
        [[nodiscard]] static bool is_modifier(xkb_keysym_t key) noexcept;

        struct xkb_context* ctx_{nullptr};
        struct xkb_keymap* map_{nullptr};
        struct xkb_state* state_{nullptr};
    };

} // namespace wayshadow
