#pragma once

#include "keypop/state.hpp"

#include <wayland-client.h>

namespace keypop {

    class Renderer {
      public:
        static constexpr double PADDING = 10.0;
        static constexpr double TOP_BOTTOM_PADDING = 5.0;
        static constexpr double RIGHT_PADDING = 60.0;

        static void
        redraw(ClientState& state, struct wl_surface* surface, struct wl_shm* shm, struct wl_buffer** buffer_out);
    };

} // namespace keypop
