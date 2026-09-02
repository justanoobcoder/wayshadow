#pragma once

#include "wayshadow/buffer.hpp"
#include "wayshadow/config.hpp"

#include <ctime>
#include <string>

namespace wayshadow {

    struct MouseState {
        bool lmb{false};
        bool rmb{false};
        bool mmb{false};
        bool last_lmb{false};
        bool last_rmb{false};
        bool last_mmb{false};
        bool has_click{false};
        int x{0};
        int y{0};
        struct timespec last_click_time{0, 0};
        std::string last_button{};
    };

    struct ModifiersState {
        bool ctrl{false};
        bool alt{false};
        bool shift{false};
        bool super{false};
    };

    struct ClientState {
        Config config{};
        TextBuffer buffer{};
        MouseState mouse{};
        ModifiersState modifiers{};

        bool running{true};
        bool window_visible{false};
        bool needs_redraw{false};
        bool overlay_enabled{true};

        struct timespec last_key_time{0, 0};

        // Key repeat state (managed by Wayland compositor / timers)
        int32_t repeat_rate{25};
        int32_t repeat_delay{600};
        uint32_t repeat_key{0};
        unsigned int repeat_timer_id{0};

        // Highlighting for key combinations
        Color current_combo_color{colors::White};
        bool use_combo_color{false};
    };

} // namespace wayshadow
