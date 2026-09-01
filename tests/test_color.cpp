#include "wayshadow/color.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

void test_color_parsing() {
    std::cout << "[TEST] Running test_color_parsing...\n";

    // 6-digit hex
    auto c1 = wayshadow::Color::from_hex("#ff0000");
    assert(c1.has_value());
    assert(std::abs(c1->r - 1.0) < 0.001);
    assert(std::abs(c1->g - 0.0) < 0.001);
    assert(std::abs(c1->b - 0.0) < 0.001);
    assert(std::abs(c1->a - 1.0) < 0.001);

    // 6-digit hex without #
    auto c2 = wayshadow::Color::from_hex("00ff00");
    assert(c2.has_value());
    assert(std::abs(c2->r - 0.0) < 0.001);
    assert(std::abs(c2->g - 1.0) < 0.001);
    assert(std::abs(c2->b - 0.0) < 0.001);

    // 8-digit hex (with alpha)
    auto c3 = wayshadow::Color::from_hex("#0000ff80");
    assert(c3.has_value());
    assert(std::abs(c3->r - 0.0) < 0.001);
    assert(std::abs(c3->g - 0.0) < 0.001);
    assert(std::abs(c3->b - 1.0) < 0.001);
    assert(std::abs(c3->a - (128.0 / 255.0)) < 0.01);

    // Invalid hex
    auto c4 = wayshadow::Color::from_hex("#12345");
    assert(!c4.has_value());

    auto c5 = wayshadow::Color::from_hex("invalid");
    assert(!c5.has_value());

    std::cout << "[TEST] test_color_parsing PASSED.\n";
}
