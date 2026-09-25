#ifndef WAYSHADOW_COLOR_HPP
#define WAYSHADOW_COLOR_HPP

#include <optional>
#include <string_view>

namespace wayshadow {

struct Color {
  double r{0.0};
  double g{0.0};
  double b{0.0};
  double a{1.0};

  [[nodiscard]] static constexpr Color FromRgba(double red, double green,
                                                double blue,
                                                double alpha = 1.0) noexcept {
    return Color{.r = red, .g = green, .b = blue, .a = alpha};
  }

  [[nodiscard]] static std::optional<Color> FromHex(
    std::string_view hex) noexcept;
};

namespace colors {
inline constexpr Color kWhite{.r = 1.0, .g = 1.0, .b = 1.0, .a = 1.0};
inline constexpr Color kBlack{.r = 0.0, .g = 0.0, .b = 0.0, .a = 1.0};
inline constexpr Color kDefaultBackground{
  .r = 0.0, .g = 0.0, .b = 0.0, .a = 0.6};
inline constexpr Color kComboGreen{.r = 0.32, .g = 0.77, .b = 0.10, .a = 1.0};
inline constexpr Color kComboBlue{.r = 0.36, .g = 0.68, .b = 0.89, .a = 1.0};
inline constexpr Color kComboPurple{.r = 0.69, .g = 0.48, .b = 0.77, .a = 1.0};
inline constexpr Color kComboOrange{.r = 0.95, .g = 0.61, .b = 0.07, .a = 1.0};
}  // namespace colors

}  // namespace wayshadow

#endif
