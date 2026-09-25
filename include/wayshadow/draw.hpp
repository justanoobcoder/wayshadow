#ifndef WAYSHADOW_DRAW_HPP
#define WAYSHADOW_DRAW_HPP

#include <wayland-client.h>

#include "wayshadow/state.hpp"

namespace wayshadow {

class Renderer {
 public:
  static constexpr double kPadding = 10.0;
  static constexpr double kTopBottomPadding = 5.0;
  static constexpr double kRightPadding = 60.0;

  static void Redraw(ClientState& state, struct wl_surface* surface,
                     struct wl_shm* shm, struct wl_buffer** buffer_out);
};

}  // namespace wayshadow

#endif
