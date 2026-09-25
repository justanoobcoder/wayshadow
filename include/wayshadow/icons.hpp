#ifndef WAYSHADOW_ICONS_HPP
#define WAYSHADOW_ICONS_HPP

#include <cairo.h>

#include <string_view>

#include "wayshadow/color.hpp"

namespace wayshadow {

class Icons {
 public:
  [[nodiscard]] static bool IsIconKey(std::string_view key) noexcept;
  static void Draw(cairo_t* cr, std::string_view key_name, double x, double y,
                   double size, const Color& color) noexcept;
  static void DrawMouse(cairo_t* cr, double x, double y, double width,
                        double height, bool lmb, bool rmb, bool mmb,
                        bool lmb_held, bool rmb_held, bool mmb_held,
                        const Color& stroke_color, const Color& fill_color,
                        const Color& hold_color) noexcept;
};

}  // namespace wayshadow

#endif
