#include "wayshadow/buffer.hpp"

#include <cassert>
#include <iostream>

void test_buffer_operations() {
    std::cout << "[TEST] Running test_buffer_operations...\n";

    wayshadow::TextBuffer buf{};
    assert(buf.empty());
    assert(buf.segment_count() == 0);

    // Appending key segments
    buf.append_or_increment("a");
    assert(buf.segment_count() == 1);
    assert(buf.full_text() == "a");

    // Repeating same key (compacts to 'a×2')
    buf.append_or_increment("a");
    assert(buf.segment_count() == 1);
    assert(
        buf.full_text()
        == "a\xc3\x97"
           "2"
    );

    buf.append_or_increment("a");
    assert(buf.segment_count() == 1);
    assert(
        buf.full_text()
        == "a\xc3\x97"
           "3"
    );

    // Adding different key
    buf.append_or_increment("b");
    assert(
        buf.full_text()
        == "a\xc3\x97"
           "3b"
    );

    // Special key adds leading space if needed
    buf.append_or_increment("Enter");
    assert(
        buf.full_text()
        == "a\xc3\x97"
           "3b Enter"
    );

    // Backspace pops "Enter"
    buf.backspace();
    assert(
        buf.full_text()
        == "a\xc3\x97"
           "3b "
    );

    // Backspace pops space token
    buf.backspace();
    assert(
        buf.full_text()
        == "a\xc3\x97"
           "3b"
    );

    // Delete word
    buf.clear();
    buf.append_raw("hello");
    buf.append_raw(" ");
    buf.append_raw("world");
    assert(buf.full_text() == "hello world");

    buf.delete_word();
    assert(buf.full_text() == "hello ");

    std::cout << "[TEST] test_buffer_operations PASSED.\n";
}
