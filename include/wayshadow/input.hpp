#pragma once

#include "wayshadow/state.hpp"

#include <functional>

struct libinput;
struct udev;

namespace wayshadow {

    class InputManager {
      public:
        using KeyCallback = std::function<void(uint32_t key, uint32_t state)>;
        using MouseButtonCallback = std::function<void(uint32_t button, uint32_t state)>;
        using MouseMotionCallback = std::function<void(double dx, double dy)>;

        InputManager(
            KeyCallback key_callback, MouseButtonCallback button_callback, MouseMotionCallback motion_callback,
            ClientState& state
        );
        InputManager(KeyCallback callback, ClientState& state);
        ~InputManager();

        InputManager(const InputManager&) = delete;
        InputManager& operator=(const InputManager&) = delete;
        InputManager(InputManager&&) noexcept;
        InputManager& operator=(InputManager&&) noexcept;

        [[nodiscard]] bool is_valid() const noexcept { return li_ != nullptr; }
        [[nodiscard]] int get_fd() const noexcept;
        void dispatch();

      private:
        struct libinput* li_{nullptr};
        struct udev* udev_{nullptr};
        KeyCallback key_callback_{};
        MouseButtonCallback button_callback_{};
        MouseMotionCallback motion_callback_{};
        ClientState* state_{nullptr};
    };

} // namespace wayshadow
