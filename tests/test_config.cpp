#include "keypop/config.hpp"

#include <cassert>
#include <iostream>

void test_config_parsing() {
    std::cout << "[TEST] Running test_config_parsing...\n";

    // CLI args parsing
    char arg0[] = "keypop";
    char arg1[] = "-b";
    char arg2[] = "#112233";
    char arg3[] = "-s";
    char arg4[] = "42";
    char arg5[] = "-g";
    char arg6[] = "600x120";
    char arg7[] = "-o";
    char arg8[] = "0.75";
    char arg9[] = "-t";
    char arg10[] = "3000";

    char* argv[] = {arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10};
    int argc = 11;

    const auto cli = keypop::CliOptions::parse(argc, argv);
    assert(cli.action == keypop::CliOptions::Action::Run);
    assert(cli.config.font_size == 42);
    assert(cli.config.width == 600);
    assert(cli.config.height == 120);
    assert(cli.config.hide_timeout_ms == 3000);
    assert(std::abs(cli.config.bg_color.a - 0.75) < 0.01);

    std::cout << "[TEST] test_config_parsing PASSED.\n";
}
