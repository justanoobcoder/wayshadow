#include "wayshadow/xkb_handler.hpp"

#include <cstring>
#include <libinput.h>
#include <utility>

namespace wayshadow {

    XkbHandler::XkbHandler() {
        ctx_ = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
        if (ctx_) {
            map_ = xkb_keymap_new_from_names(ctx_, nullptr, XKB_KEYMAP_COMPILE_NO_FLAGS);
            if (map_) {
                state_ = xkb_state_new(map_);
            }
        }
    }

    XkbHandler::~XkbHandler() {
        if (state_)
            xkb_state_unref(state_);
        if (map_)
            xkb_keymap_unref(map_);
        if (ctx_)
            xkb_context_unref(ctx_);
    }

    XkbHandler::XkbHandler(XkbHandler&& other) noexcept
        : ctx_(std::exchange(other.ctx_, nullptr)), map_(std::exchange(other.map_, nullptr)),
          state_(std::exchange(other.state_, nullptr)) {}

    XkbHandler& XkbHandler::operator=(XkbHandler&& other) noexcept {
        if (this != &other) {
            if (state_)
                xkb_state_unref(state_);
            if (map_)
                xkb_keymap_unref(map_);
            if (ctx_)
                xkb_context_unref(ctx_);

            ctx_ = std::exchange(other.ctx_, nullptr);
            map_ = std::exchange(other.map_, nullptr);
            state_ = std::exchange(other.state_, nullptr);
        }
        return *this;
    }

    const char* XkbHandler::get_key_symbol(xkb_keysym_t keysym) noexcept {
        switch (keysym) {
        case XKB_KEY_Return:
        case XKB_KEY_KP_Enter:
            return "Enter";
        case XKB_KEY_Tab:
        case XKB_KEY_ISO_Left_Tab:
            return "Tab";
        case XKB_KEY_Escape:
            return "Esc";
        case XKB_KEY_Up:
            return "Up";
        case XKB_KEY_Down:
            return "Down";
        case XKB_KEY_Left:
            return "Left";
        case XKB_KEY_Right:
            return "Right";
        case XKB_KEY_Control_L:
        case XKB_KEY_Control_R:
            return "Ctrl";
        case XKB_KEY_Alt_L:
        case XKB_KEY_Alt_R:
            return "Alt";
        case XKB_KEY_Super_L:
        case XKB_KEY_Super_R:
            return "Super";
        case XKB_KEY_Shift_L:
        case XKB_KEY_Shift_R:
            return "Shift";
        case XKB_KEY_space:
            return "Space";
        case XKB_KEY_BackSpace:
            return "Backspace";
        case XKB_KEY_Delete:
            return "Del";
        case XKB_KEY_Home:
            return "Home";
        case XKB_KEY_End:
            return "End";
        case XKB_KEY_Prior:
            return "PgUp";
        case XKB_KEY_Next:
            return "PgDn";
        case XKB_KEY_Caps_Lock:
            return "Caps";
        case XKB_KEY_Num_Lock:
            return "Num";
        case XKB_KEY_F1:
            return "F1";
        case XKB_KEY_F2:
            return "F2";
        case XKB_KEY_F3:
            return "F3";
        case XKB_KEY_F4:
            return "F4";
        case XKB_KEY_F5:
            return "F5";
        case XKB_KEY_F6:
            return "F6";
        case XKB_KEY_F7:
            return "F7";
        case XKB_KEY_F8:
            return "F8";
        case XKB_KEY_F9:
            return "F9";
        case XKB_KEY_F10:
            return "F10";
        case XKB_KEY_F11:
            return "F11";
        case XKB_KEY_F12:
            return "F12";
        case XKB_KEY_XF86AudioLowerVolume:
            return "Vol-";
        case XKB_KEY_XF86AudioRaiseVolume:
            return "Vol+";
        case XKB_KEY_XF86AudioMute:
            return "Mute";
        case XKB_KEY_XF86MonBrightnessUp:
            return "Bri+";
        case XKB_KEY_XF86MonBrightnessDown:
            return "Bri-";
        case XKB_KEY_XF86AudioPlay:
            return "Play";
        case XKB_KEY_XF86AudioPrev:
            return "Prev";
        case XKB_KEY_XF86AudioNext:
            return "Next";
        default:
            return nullptr;
        }
    }

    bool XkbHandler::is_modifier(xkb_keysym_t key) noexcept {
        return (
            key == XKB_KEY_Control_L
            || key == XKB_KEY_Control_R
            || key == XKB_KEY_Alt_L
            || key == XKB_KEY_Alt_R
            || key == XKB_KEY_Super_L
            || key == XKB_KEY_Super_R
            || key == XKB_KEY_Shift_L
            || key == XKB_KEY_Shift_R
        );
    }

    void XkbHandler::process_repeat_key(ClientState& state, uint32_t key) { process_key_down(state, key); }

    void XkbHandler::process_key_down(ClientState& state, uint32_t key) {
        // Linux evdev scancode to XKB keycode mapping requires adding 8
        const uint32_t xkb_keycode = key + 8;
        xkb_state_update_key(state_, xkb_keycode, XKB_KEY_DOWN);
        const xkb_keysym_t keysym = xkb_state_key_get_one_sym(state_, xkb_keycode);

        const bool is_mod = is_modifier(keysym);
        if (keysym == XKB_KEY_Control_L || keysym == XKB_KEY_Control_R)
            state.modifiers.ctrl = true;
        if (keysym == XKB_KEY_Alt_L || keysym == XKB_KEY_Alt_R)
            state.modifiers.alt = true;
        if (keysym == XKB_KEY_Shift_L || keysym == XKB_KEY_Shift_R)
            state.modifiers.shift = true;
        if (keysym == XKB_KEY_Super_L || keysym == XKB_KEY_Super_R)
            state.modifiers.super = true;

        if (state.overlay_enabled) {
            clock_gettime(CLOCK_MONOTONIC, &state.last_key_time);

            if (keysym == XKB_KEY_BackSpace) {
                if (state.modifiers.ctrl) {
                    state.buffer.reset_repeat_state();
                    state.buffer.delete_word();
                } else {
                    state.buffer.append_or_increment("\xe2\x8c\xab"); // ⌫ backspace glyph
                }
            } else if (state.modifiers.ctrl && keysym == XKB_KEY_w) {
                state.buffer.reset_repeat_state();
                state.buffer.delete_word();
            } else if (!is_mod) {
                std::string combined;
                if (state.modifiers.ctrl)
                    combined += "Ctrl+";
                if (state.modifiers.alt)
                    combined += "Alt+";
                if (state.modifiers.super)
                    combined += "Super+";

                const char* sym = get_key_symbol(keysym);
                char key_str[32] = {0};

                if (sym) {
                    std::strncpy(key_str, sym, sizeof(key_str) - 1);
                } else if (keysym >= 0x20 && keysym <= 0x7E) {
                    key_str[0] = static_cast<char>(keysym);
                    key_str[1] = '\0';
                } else {
                    xkb_state_key_get_utf8(state_, xkb_keycode, key_str, sizeof(key_str));
                    if (std::strlen(key_str) == 0 || static_cast<unsigned char>(key_str[0]) < 32) {
                        if (keysym < 256 && keysym >= 32) {
                            key_str[0] = static_cast<char>(keysym);
                            key_str[1] = '\0';
                        } else {
                            key_str[0] = '\0';
                        }
                    }
                }

                combined += key_str;

                if (!combined.empty()) {
                    state.use_combo_color = false;
                    if (state.modifiers.ctrl && !state.modifiers.alt && !state.modifiers.super) {
                        if (keysym == XKB_KEY_c || keysym == XKB_KEY_v || keysym == XKB_KEY_x || keysym == XKB_KEY_z) {
                            state.current_combo_color = colors::ComboGreen;
                            state.use_combo_color = true;
                        } else {
                            state.current_combo_color = colors::ComboBlue;
                            state.use_combo_color = true;
                        }
                    } else if (state.modifiers.alt && !state.modifiers.ctrl) {
                        state.current_combo_color = colors::ComboPurple;
                        state.use_combo_color = true;
                    } else if (state.modifiers.super) {
                        state.current_combo_color = colors::ComboOrange;
                        state.use_combo_color = true;
                    }

                    state.buffer.append_or_increment(combined);
                }
            }
            state.needs_redraw = true;
        }
    }

    void XkbHandler::handle_key_event(ClientState& state, uint32_t key, uint32_t state_val) {
        const uint32_t xkb_keycode = key + 8;
        const xkb_keysym_t raw_sym = xkb_state_key_get_one_sym(state_, xkb_keycode);
        const bool is_mod_key = is_modifier(raw_sym);

        if (state_val == LIBINPUT_KEY_STATE_PRESSED) {
            if (is_mod_key) {
                if ((raw_sym == XKB_KEY_Shift_L || raw_sym == XKB_KEY_Shift_R) && state.modifiers.shift)
                    return;
                if ((raw_sym == XKB_KEY_Control_L || raw_sym == XKB_KEY_Control_R) && state.modifiers.ctrl)
                    return;
                if ((raw_sym == XKB_KEY_Alt_L || raw_sym == XKB_KEY_Alt_R) && state.modifiers.alt)
                    return;
                if ((raw_sym == XKB_KEY_Super_L || raw_sym == XKB_KEY_Super_R) && state.modifiers.super)
                    return;
            }

            process_key_down(state, key);

            const xkb_keysym_t keysym = xkb_state_key_get_one_sym(state_, xkb_keycode);
            if (!is_modifier(keysym) && state.repeat_rate > 0 && state.repeat_delay > 0) {
                state.repeat_key = key;
            }
        } else {
            if (state.repeat_key == key) {
                state.repeat_key = 0;
            }

            xkb_state_update_key(state_, xkb_keycode, XKB_KEY_UP);
            const xkb_keysym_t keysym = xkb_state_key_get_one_sym(state_, xkb_keycode);
            if (keysym == XKB_KEY_Control_L || keysym == XKB_KEY_Control_R)
                state.modifiers.ctrl = false;
            if (keysym == XKB_KEY_Alt_L || keysym == XKB_KEY_Alt_R)
                state.modifiers.alt = false;
            if (keysym == XKB_KEY_Shift_L || keysym == XKB_KEY_Shift_R)
                state.modifiers.shift = false;
            if (keysym == XKB_KEY_Super_L || keysym == XKB_KEY_Super_R)
                state.modifiers.super = false;

            state.buffer.reset_repeat_state();
        }
    }

} // namespace wayshadow
