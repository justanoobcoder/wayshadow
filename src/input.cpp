#include "wayshadow/input.hpp"

#include <algorithm>
#include <cstdio>
#include <errno.h>
#include <fcntl.h>
#include <libinput.h>
#include <libudev.h>
#include <linux/input.h>
#include <unistd.h>
#include <utility>

namespace wayshadow {

    namespace {

        int open_restricted(const char* path, int flags, void* /*user_data*/) {
            const int fd = open(path, flags);
            return fd < 0 ? -errno : fd;
        }

        void close_restricted(int fd, void* /*user_data*/) { close(fd); }

        const struct libinput_interface interface = {
            .open_restricted = open_restricted,
            .close_restricted = close_restricted,
        };

    } // namespace

    InputManager::InputManager(
        KeyCallback key_callback, MouseButtonCallback button_callback, MouseMotionCallback motion_callback,
        ClientState& state
    )
        : key_callback_(std::move(key_callback)), button_callback_(std::move(button_callback)),
          motion_callback_(std::move(motion_callback)), state_(&state) {
        udev_ = udev_new();
        if (!udev_) {
            std::fprintf(stderr, "Failed to initialize udev\n");
            return;
        }

        li_ = libinput_udev_create_context(&interface, nullptr, udev_);
        if (!li_) {
            std::fprintf(stderr, "Failed to initialize libinput\n");
            udev_unref(udev_);
            udev_ = nullptr;
            return;
        }

        libinput_udev_assign_seat(li_, "seat0");
    }

    InputManager::InputManager(KeyCallback callback, ClientState& state)
        : InputManager(std::move(callback), nullptr, nullptr, state) {}

    InputManager::~InputManager() {
        if (li_)
            libinput_unref(li_);
        if (udev_)
            udev_unref(udev_);
    }

    InputManager::InputManager(InputManager&& other) noexcept
        : li_(std::exchange(other.li_, nullptr)), udev_(std::exchange(other.udev_, nullptr)),
          key_callback_(std::move(other.key_callback_)), button_callback_(std::move(other.button_callback_)),
          motion_callback_(std::move(other.motion_callback_)), state_(std::exchange(other.state_, nullptr)) {}

    InputManager& InputManager::operator=(InputManager&& other) noexcept {
        if (this != &other) {
            if (li_)
                libinput_unref(li_);
            if (udev_)
                udev_unref(udev_);

            li_ = std::exchange(other.li_, nullptr);
            udev_ = std::exchange(other.udev_, nullptr);
            key_callback_ = std::move(other.key_callback_);
            button_callback_ = std::move(other.button_callback_);
            motion_callback_ = std::move(other.motion_callback_);
            state_ = std::exchange(other.state_, nullptr);
        }
        return *this;
    }

    int InputManager::get_fd() const noexcept { return li_ ? libinput_get_fd(li_) : -1; }

    void InputManager::dispatch() {
        if (!li_)
            return;

        libinput_dispatch(li_);

        struct libinput_event* event = nullptr;
        while ((event = libinput_get_event(li_))) {
            const auto type = libinput_event_get_type(event);

            if (type == LIBINPUT_EVENT_KEYBOARD_KEY) {
                auto* k = libinput_event_get_keyboard_event(event);
                const uint32_t key = libinput_event_keyboard_get_key(k);
                const uint32_t key_state = libinput_event_keyboard_get_key_state(k);

                if (key_callback_) {
                    key_callback_(key, key_state);
                }
            } else if (type == LIBINPUT_EVENT_POINTER_BUTTON) {
                auto* p = libinput_event_get_pointer_event(event);
                const uint32_t button = libinput_event_pointer_get_button(p);
                const uint32_t button_state = libinput_event_pointer_get_button_state(p);

                const bool pressed = (button_state == LIBINPUT_BUTTON_STATE_PRESSED);
                if (button == BTN_LEFT) {
                    state_->mouse.lmb = pressed;
                } else if (button == BTN_RIGHT) {
                    state_->mouse.rmb = pressed;
                } else if (button == BTN_MIDDLE) {
                    state_->mouse.mmb = pressed;
                }

                if (pressed) {
                    clock_gettime(CLOCK_MONOTONIC, &state_->mouse.last_click_time);
                    clock_gettime(CLOCK_MONOTONIC, &state_->last_key_time);

                    std::string btn_str;
                    if (state_->mouse.lmb)
                        btn_str += "LMB ";
                    if (state_->mouse.rmb)
                        btn_str += "RMB ";
                    if (state_->mouse.mmb)
                        btn_str += "MMB ";
                    if (btn_str.empty()) {
                        if (button == BTN_SIDE || button == BTN_BACK) {
                            btn_str = "Back ";
                        } else if (button == BTN_EXTRA || button == BTN_FORWARD) {
                            btn_str = "Forward ";
                        } else {
                            btn_str = "Mouse ";
                        }
                    }
                    state_->mouse.last_button = btn_str;
                }

                if (button_callback_) {
                    button_callback_(button, button_state);
                }
            } else if (type == LIBINPUT_EVENT_POINTER_MOTION) {
                auto* p = libinput_event_get_pointer_event(event);
                const double dx = libinput_event_pointer_get_dx(p);
                const double dy = libinput_event_pointer_get_dy(p);

                state_->mouse.x += static_cast<int>(dx);
                state_->mouse.y += static_cast<int>(dy);

                state_->mouse.x = std::clamp(state_->mouse.x, 0, 3840);
                state_->mouse.y = std::clamp(state_->mouse.y, 0, 2160);

                if (motion_callback_) {
                    motion_callback_(dx, dy);
                }
            }

            libinput_event_destroy(event);
        }
    }

} // namespace wayshadow
