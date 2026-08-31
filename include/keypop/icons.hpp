#pragma once

#include "keypop/color.hpp"

#include <cairo.h>
#include <string_view>

namespace keypop {

    class Icons {
      public:
        [[nodiscard]] static bool is_icon_key(std::string_view key) noexcept;
        static void
        draw(cairo_t* cr, std::string_view key_name, double x, double y, double size, const Color& color) noexcept;
    };

} // namespace keypop
