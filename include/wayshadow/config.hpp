#pragma once

#include "wayshadow/color.hpp"

#include <filesystem>
#include <string>

namespace wayshadow {

    struct Config {
        Color bg_color{colors::DefaultBackground};
        Color text_color{colors::White};
        int font_size{65};
        int width{840};
        int height{130};
        int hide_timeout_ms{2000};

        static constexpr int MIN_WIDTH = 100;
        static constexpr int MIN_HEIGHT = 50;
        static constexpr int MIN_FONT_SIZE = 10;
        static constexpr int DEFAULT_HIDE_TIMEOUT_MS = 2000;

        static Config load_from_file(const std::filesystem::path& path);
        static Config load_default_config();
    };

    struct CliOptions {
        enum class Action { Run, ShowHelp, ShowVersion, Error };

        Action action{Action::Run};
        Config config{};
        std::string error_message{};

        static CliOptions parse(int argc, char* argv[]);
        static void print_usage(std::string_view program_name);
        static void print_version();
    };

} // namespace wayshadow
