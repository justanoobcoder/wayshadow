#include "wayshadow/color.hpp"

#include <charconv>
#include <cstdint>

namespace wayshadow {

namespace {
[[nodiscard]] std::optional<uint8_t> ParseHexByte(
  std::string_view sv) noexcept {
  if (sv.size() != 2) {
    return std::nullopt;
  }
  uint8_t val = 0;
  const auto [ptr, ec] =
    std::from_chars(sv.data(), sv.data() + sv.size(), val, 16);
  if (ec != std::errc{} || ptr != sv.data() + sv.size()) {
    return std::nullopt;
  }
  return val;
}
}  // namespace

std::optional<Color> Color::FromHex(std::string_view hex) noexcept {
  if (hex.starts_with('#')) {
    hex.remove_prefix(1);
  }

  if (hex.length() != 6 && hex.length() != 8) {
    return std::nullopt;
  }

  const auto r_opt = ParseHexByte(hex.substr(0, 2));
  const auto g_opt = ParseHexByte(hex.substr(2, 2));
  const auto b_opt = ParseHexByte(hex.substr(4, 2));

  if (!r_opt || !g_opt || !b_opt) {
    return std::nullopt;
  }

  uint8_t a_val = 255;
  if (hex.length() == 8) {
    const auto a_opt = ParseHexByte(hex.substr(6, 2));
    if (!a_opt) {
      return std::nullopt;
    }
    a_val = *a_opt;
  }

  return Color{.r = static_cast<double>(*r_opt) / 255.0,
               .g = static_cast<double>(*g_opt) / 255.0,
               .b = static_cast<double>(*b_opt) / 255.0,
               .a = static_cast<double>(a_val) / 255.0};
}

}  // namespace wayshadow
