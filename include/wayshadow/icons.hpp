#pragma once

#include "wayshadow/color.hpp"

#include <cairo.h>
#include <string_view>

namespace wayshadow {

    class Icons {
      public:
        [[nodiscard]] static bool is_icon_key(std::string_view key) noexcept;
        static void
        draw(cairo_t* cr, std::string_view key_name, double x, double y, double size, const Color& color) noexcept;
        static void draw_mouse(
            cairo_t* cr, double x, double y, double width, double height, bool lmb, bool rmb, bool mmb, bool lmb_held,
            bool rmb_held, bool mmb_held, const Color& stroke_color, const Color& fill_color, const Color& hold_color
        ) noexcept;
    };

} // namespace wayshadow
