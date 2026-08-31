#include "keypop/config.hpp"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

static std::filesystem::path write_tmp_conf(const std::string& content) {
    char buf[64];
    std::snprintf(buf, sizeof(buf), "/tmp/keypop_test_%p.conf", static_cast<const void*>(content.c_str()));
    std::ofstream f(buf);
    f << content;
    return buf;
}

void test_config_file_full() {
    std::cout << "[TEST] Running test_config_file_full...\n";

    const std::string ini = "[settings]\n"
                            "background = #1e1e2e\n"
                            "foreground = #cdd6f4\n"
                            "opacity = 0.8\n"
                            "font_size = 70\n"
                            "geometry = 900x140\n"
                            "hide_timeout = 2500\n";

    const auto path = write_tmp_conf(ini);
    const keypop::Config cfg = keypop::Config::load_from_file(path);
    std::filesystem::remove(path);

    assert(std::abs(cfg.bg_color.r - (0x1e / 255.0)) < 0.01);
    assert(std::abs(cfg.bg_color.g - (0x1e / 255.0)) < 0.01);
    assert(std::abs(cfg.bg_color.b - (0x2e / 255.0)) < 0.01);
    assert(std::abs(cfg.bg_color.a - 0.8) < 0.01);

    assert(std::abs(cfg.text_color.r - (0xcd / 255.0)) < 0.01);
    assert(std::abs(cfg.text_color.g - (0xd6 / 255.0)) < 0.01);
    assert(std::abs(cfg.text_color.b - (0xf4 / 255.0)) < 0.01);

    assert(cfg.font_size == 70);
    assert(cfg.width == 900);
    assert(cfg.height == 140);
    assert(cfg.hide_timeout_ms == 2500);

    std::cout << "[TEST] test_config_file_full PASSED.\n";
}

void test_config_file_partial() {
    std::cout << "[TEST] Running test_config_file_partial...\n";

    const std::string ini = "[settings]\n"
                            "font_size = 50\n"
                            "hide_timeout = 500\n";

    const auto path = write_tmp_conf(ini);
    const keypop::Config cfg = keypop::Config::load_from_file(path);
    std::filesystem::remove(path);

    assert(cfg.font_size == 50);
    assert(cfg.hide_timeout_ms == 500);

    assert(cfg.width == 840);
    assert(cfg.height == 130);
    assert(std::abs(cfg.bg_color.a - keypop::colors::DefaultBackground.a) < 0.01);

    std::cout << "[TEST] test_config_file_partial PASSED.\n";
}

void test_config_file_bg_6hex_preserves_default_alpha() {
    std::cout << "[TEST] Running test_config_file_bg_6hex_preserves_default_alpha...\n";

    const std::string ini = "[settings]\n"
                            "background = #ff0000\n";

    const auto path = write_tmp_conf(ini);
    const keypop::Config cfg = keypop::Config::load_from_file(path);
    std::filesystem::remove(path);

    assert(std::abs(cfg.bg_color.r - 1.0) < 0.01);
    assert(std::abs(cfg.bg_color.g - 0.0) < 0.01);
    assert(std::abs(cfg.bg_color.b - 0.0) < 0.01);
    assert(std::abs(cfg.bg_color.a - keypop::colors::DefaultBackground.a) < 0.01);

    std::cout << "[TEST] test_config_file_bg_6hex_preserves_default_alpha PASSED.\n";
}

void test_config_file_bg_8hex_uses_embedded_alpha() {
    std::cout << "[TEST] Running test_config_file_bg_8hex_uses_embedded_alpha...\n";

    const std::string ini = "[settings]\n"
                            "background = #0000ff80\n";

    const auto path = write_tmp_conf(ini);
    const keypop::Config cfg = keypop::Config::load_from_file(path);
    std::filesystem::remove(path);

    assert(std::abs(cfg.bg_color.r - 0.0) < 0.01);
    assert(std::abs(cfg.bg_color.g - 0.0) < 0.01);
    assert(std::abs(cfg.bg_color.b - 1.0) < 0.01);
    assert(std::abs(cfg.bg_color.a - (0x80 / 255.0)) < 0.01);

    std::cout << "[TEST] test_config_file_bg_8hex_uses_embedded_alpha PASSED.\n";
}

void test_config_file_opacity_clamp() {
    std::cout << "[TEST] Running test_config_file_opacity_clamp...\n";

    const std::string ini_high = "[settings]\n"
                                 "opacity = 5.0\n";
    auto path = write_tmp_conf(ini_high);
    auto cfg = keypop::Config::load_from_file(path);
    std::filesystem::remove(path);
    assert(std::abs(cfg.bg_color.a - 1.0) < 0.001);

    const std::string ini_low = "[settings]\n"
                                "opacity = -1.0\n";
    path = write_tmp_conf(ini_low);
    cfg = keypop::Config::load_from_file(path);
    std::filesystem::remove(path);
    assert(std::abs(cfg.bg_color.a - 0.0) < 0.001);

    std::cout << "[TEST] test_config_file_opacity_clamp PASSED.\n";
}

void test_config_file_clamp_minimums() {
    std::cout << "[TEST] Running test_config_file_clamp_minimums...\n";

    const std::string ini = "[settings]\n"
                            "font_size = 1\n"
                            "geometry = 1x1\n";

    const auto path = write_tmp_conf(ini);
    const keypop::Config cfg = keypop::Config::load_from_file(path);
    std::filesystem::remove(path);

    assert(cfg.font_size == keypop::Config::MIN_FONT_SIZE);
    assert(cfg.width == keypop::Config::MIN_WIDTH);
    assert(cfg.height == keypop::Config::MIN_HEIGHT);

    std::cout << "[TEST] test_config_file_clamp_minimums PASSED.\n";
}

void test_config_file_negative_hide_timeout_uses_default() {
    std::cout << "[TEST] Running test_config_file_negative_hide_timeout_uses_default...\n";

    const std::string ini = "[settings]\n"
                            "hide_timeout = -500\n";

    const auto path = write_tmp_conf(ini);
    const keypop::Config cfg = keypop::Config::load_from_file(path);
    std::filesystem::remove(path);

    assert(cfg.hide_timeout_ms == keypop::Config::DEFAULT_HIDE_TIMEOUT_MS);

    std::cout << "[TEST] test_config_file_negative_hide_timeout_uses_default PASSED.\n";
}

void test_config_file_zero_hide_timeout_allowed() {
    std::cout << "[TEST] Running test_config_file_zero_hide_timeout_allowed...\n";

    const std::string ini = "[settings]\n"
                            "hide_timeout = 0\n";

    const auto path = write_tmp_conf(ini);
    const keypop::Config cfg = keypop::Config::load_from_file(path);
    std::filesystem::remove(path);

    assert(cfg.hide_timeout_ms == 0);

    std::cout << "[TEST] test_config_file_zero_hide_timeout_allowed PASSED.\n";
}

void test_config_file_missing_returns_defaults() {
    std::cout << "[TEST] Running test_config_file_missing_returns_defaults...\n";

    const keypop::Config defaults{};
    const keypop::Config cfg = keypop::Config::load_from_file("/tmp/keypop_nonexistent_file_12345.conf");

    assert(cfg.font_size == defaults.font_size);
    assert(cfg.width == defaults.width);
    assert(cfg.height == defaults.height);
    assert(cfg.hide_timeout_ms == defaults.hide_timeout_ms);
    assert(std::abs(cfg.bg_color.a - defaults.bg_color.a) < 0.001);

    std::cout << "[TEST] test_config_file_missing_returns_defaults PASSED.\n";
}

void test_config_parsing() {
    std::cout << "[TEST] Running test_config_parsing...\n";

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

void test_config_file_loading() {
    test_config_file_full();
    test_config_file_partial();
    test_config_file_bg_6hex_preserves_default_alpha();
    test_config_file_bg_8hex_uses_embedded_alpha();
    test_config_file_opacity_clamp();
    test_config_file_clamp_minimums();
    test_config_file_negative_hide_timeout_uses_default();
    test_config_file_zero_hide_timeout_allowed();
    test_config_file_missing_returns_defaults();
}
