#ifndef WAYSHADOW_CONFIG_HPP
#define WAYSHADOW_CONFIG_HPP

#include <filesystem>
#include <string>

#include "wayshadow/color.hpp"

namespace wayshadow {

struct Config {
  Color bg_color{colors::kDefaultBackground};
  Color text_color{colors::kWhite};
  Color hold_color{.r = 0.9, .g = 0.2, .b = 0.2, .a = 1.0};
  int font_size{65};
  int width{840};
  int height{130};
  int hide_timeout_ms{2000};

  static constexpr int kMinWidth = 100;
  static constexpr int kMinHeight = 50;
  static constexpr int kMinFontSize = 10;
  static constexpr int kDefaultHideTimeoutMs = 2000;

  static Config LoadFromFile(const std::filesystem::path& path);
  static Config LoadDefaultConfig();
};

struct CliOptions {
  enum class Action { kRun, kShowHelp, kShowVersion, kError };

  Action action{Action::kRun};
  Config config{};
  std::string error_message;

  static CliOptions Parse(int argc, char* argv[]);
  static void PrintUsage(std::string_view program_name);
  static void PrintVersion();
};

}  // namespace wayshadow

#endif
