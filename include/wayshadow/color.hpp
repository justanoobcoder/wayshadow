#pragma once

#include <optional>
#include <string_view>

namespace wayshadow {

    struct Color {
        double r{0.0};
        double g{0.0};
        double b{0.0};
        double a{1.0};

        [[nodiscard]] static constexpr Color
        from_rgba(double red, double green, double blue, double alpha = 1.0) noexcept {
            return Color{red, green, blue, alpha};
        }

        [[nodiscard]] static std::optional<Color> from_hex(std::string_view hex) noexcept;
    };

    namespace colors {
        inline constexpr Color White{1.0, 1.0, 1.0, 1.0};
        inline constexpr Color Black{0.0, 0.0, 0.0, 1.0};
        inline constexpr Color DefaultBackground{0.0, 0.0, 0.0, 0.6};
        inline constexpr Color ComboGreen{0.32, 0.77, 0.10, 1.0};
        inline constexpr Color ComboBlue{0.36, 0.68, 0.89, 1.0};
        inline constexpr Color ComboPurple{0.69, 0.48, 0.77, 1.0};
        inline constexpr Color ComboOrange{0.95, 0.61, 0.07, 1.0};
    } // namespace colors

} // namespace wayshadow
