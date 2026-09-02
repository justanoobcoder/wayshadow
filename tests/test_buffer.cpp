#include "wayshadow/buffer.hpp"
#include "wayshadow/state.hpp"

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

void test_mouse_state() {
    std::cout << "[TEST] Running test_mouse_state...\n";

    wayshadow::ClientState state{};
    assert(!state.window_visible);
    assert(state.mouse.last_button.empty());
    assert(!state.mouse.lmb);
    assert(!state.mouse.rmb);
    assert(!state.mouse.mmb);
    assert(!state.mouse.back);
    assert(!state.mouse.forward);
    assert(!state.mouse.last_lmb);
    assert(!state.mouse.last_rmb);
    assert(!state.mouse.last_mmb);
    assert(!state.mouse.last_back);
    assert(!state.mouse.last_forward);
    assert(!state.mouse.has_click);

    state.mouse.lmb = true;
    state.mouse.last_lmb = true;
    state.mouse.has_click = true;
    state.mouse.last_button = "LMB ";
    assert(state.mouse.last_button == "LMB ");
    assert(state.mouse.last_lmb);
    assert(state.mouse.has_click);

    // Resetting mouse button on release keeps last_lmb and has_click for display
    state.mouse.lmb = false;
    assert(state.mouse.last_lmb);
    assert(state.mouse.has_click);

    // Side buttons (Back / Forward)
    state.mouse.back = true;
    state.mouse.last_back = true;
    assert(state.mouse.back);
    assert(state.mouse.last_back);

    state.mouse.forward = true;
    state.mouse.last_forward = true;
    assert(state.mouse.forward);
    assert(state.mouse.last_forward);

    // Clearing on hide / new key
    state.mouse.last_button.clear();
    state.mouse.last_lmb = false;
    state.mouse.last_back = false;
    state.mouse.last_forward = false;
    state.mouse.has_click = false;
    assert(state.mouse.last_button.empty());
    assert(!state.mouse.has_click);

    std::cout << "[TEST] test_mouse_state PASSED.\n";
}
