#include "wayshadow/config.hpp"

#include <glib.h>
#include <unistd.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "wayshadow/version.hpp"

namespace wayshadow {

Config Config::LoadFromFile(const std::filesystem::path& path) {
  Config cfg{};
  GKeyFile* keyfile = g_key_file_new();

  if (g_key_file_load_from_file(keyfile, path.c_str(), G_KEY_FILE_NONE,
                                nullptr)) {
    gchar* bg =
      g_key_file_get_string(keyfile, "settings", "background", nullptr);
    if (bg) {
      if (auto col = Color::FromHex(bg)) {
        // Preserve default alpha if 6-hex was passed
        if (std::string_view(bg).starts_with('#')
              ? (std::string_view(bg).length() == 7)
              : (std::string_view(bg).length() == 6)) {
          col->a = cfg.bg_color.a;
        }
        cfg.bg_color = *col;
      }
      g_free(bg);
    }

    gchar* fg =
      g_key_file_get_string(keyfile, "settings", "foreground", nullptr);
    if (fg) {
      if (auto col = Color::FromHex(fg)) {
        cfg.text_color = *col;
      }
      g_free(fg);
    }

    gchar* h_col =
      g_key_file_get_string(keyfile, "settings", "hold_color", nullptr);
    if (h_col) {
      if (auto col = Color::FromHex(h_col)) {
        cfg.hold_color = *col;
      }
      g_free(h_col);
    }

    if (g_key_file_has_key(keyfile, "settings", "font_size", nullptr)) {
      cfg.font_size = std::max(
        kMinFontSize,
        g_key_file_get_integer(keyfile, "settings", "font_size", nullptr));
    }

    gchar* geo =
      g_key_file_get_string(keyfile, "settings", "geometry", nullptr);
    if (geo) {
      int w = 0;
      int h = 0;
      if (std::sscanf(geo, "%dx%d", &w, &h) == 2) {
        cfg.width = std::max(kMinWidth, w);
        cfg.height = std::max(kMinHeight, h);
      }
      g_free(geo);
    }

    if (g_key_file_has_key(keyfile, "settings", "opacity", nullptr)) {
      cfg.bg_color.a = std::clamp(
        g_key_file_get_double(keyfile, "settings", "opacity", nullptr), 0.0,
        1.0);
    }

    if (g_key_file_has_key(keyfile, "settings", "hide_timeout", nullptr)) {
      const int t =
        g_key_file_get_integer(keyfile, "settings", "hide_timeout", nullptr);
      cfg.hide_timeout_ms = (t < 0) ? kDefaultHideTimeoutMs : t;
    }
  }

  g_key_file_free(keyfile);
  return cfg;
}

Config Config::LoadDefaultConfig() {
  const char* config_dir = g_get_user_config_dir();
  if (!config_dir) {
    return Config{};
  }
  const auto conf_path =
    std::filesystem::path(config_dir) / "wayshadow" / "wayshadow.conf";
  return LoadFromFile(conf_path);
}

CliOptions CliOptions::Parse(int argc, char* argv[]) {
  CliOptions result{};
  result.config = Config::LoadDefaultConfig();

  optind = 1;  // Reset getopt
  int opt = 0;
  while ((opt = getopt(argc, argv, "b:c:s:g:o:t:vh")) != -1) {
    switch (opt) {
      case 'b':
        if (auto col = Color::FromHex(optarg)) {
          if (std::string_view(optarg).starts_with('#')
                ? (std::string_view(optarg).length() == 7)
                : (std::string_view(optarg).length() == 6)) {
            col->a = result.config.bg_color.a;
          }
          result.config.bg_color = *col;
        } else {
          result.action = Action::kError;
          result.error_message = "Invalid background hex color format: ";
          result.error_message += optarg;
          return result;
        }
        break;
      case 'c':
        if (auto col = Color::FromHex(optarg)) {
          result.config.text_color = *col;
        } else {
          result.action = Action::kError;
          result.error_message = "Invalid foreground hex color format: ";
          result.error_message += optarg;
          return result;
        }
        break;
      case 's':
        result.config.font_size =
          std::max(Config::kMinFontSize, std::atoi(optarg));
        break;
      case 'g': {
        int w = 0;
        int h = 0;
        if (std::sscanf(optarg, "%dx%d", &w, &h) == 2) {
          result.config.width = std::max(Config::kMinWidth, w);
          result.config.height = std::max(Config::kMinHeight, h);
        } else {
          result.action = Action::kError;
          result.error_message =
            "Invalid geometry format. Expected WxH (e.g. 840x130)";
          return result;
        }
        break;
      }
      case 'o': {
        const double opacity = std::atof(optarg);
        result.config.bg_color.a = std::clamp(opacity, 0.0, 1.0);
        break;
      }
      case 't': {
        const int t = std::atoi(optarg);
        if (t >= 0) {
          result.config.hide_timeout_ms = t;
        }
        break;
      }
      case 'v':
        result.action = Action::kShowVersion;
        return result;
      case 'h':
        result.action = Action::kShowHelp;
        return result;
      default:
        result.action = Action::kError;
        result.error_message = "Unknown command line option";
        return result;
    }
  }

  return result;
}

void CliOptions::PrintUsage(std::string_view program_name) {
  std::cout
    << "Usage: " << program_name << " [options]\n"
    << "Options:\n"
    << "  -b <color>   Set background color (e.g. #000000 or 000000)\n"
    << "  -c <color>   Set text color (e.g. #FFFFFF or FFFFFF)\n"
    << "  -s <size>    Set font size (default: 65)\n"
    << "  -g <WxH>     Set window size (default: 840x130)\n"
    << "  -o <opacity> Set background opacity (0.0 - 1.0, default: 0.6)\n"
    << "  -t <ms>      Set hide timeout in milliseconds (0 = never hide, "
       "default: 2000)\n"
    << "  -v           Get version info\n"
    << "  -h           Show this help\n";
}

void CliOptions::PrintVersion() {
  std::cout << "wayshadow v" << kAppVersion << " (build: " << kGitCommit << ", "
            << kBuildDate << ")\n";
}

}  // namespace wayshadow
