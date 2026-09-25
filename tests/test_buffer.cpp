#include <cassert>
#include <iostream>

#include "wayshadow/buffer.hpp"
#include "wayshadow/state.hpp"

extern void TestBufferOperations() {
  std::cout << "[TEST] Running test_buffer_operations...\n";

  wayshadow::TextBuffer buf{};
  assert(buf.Empty());
  assert(buf.SegmentCount() == 0);

  // Appending key segments
  buf.AppendOrIncrement("a");
  assert(buf.SegmentCount() == 1);
  assert(buf.FullText() == "a");

  // Repeating same key (compacts to 'a×2')
  buf.AppendOrIncrement("a");
  assert(buf.SegmentCount() == 1);
  assert(buf.FullText() ==
         "a\xc3\x97"
         "2");

  buf.AppendOrIncrement("a");
  assert(buf.SegmentCount() == 1);
  assert(buf.FullText() ==
         "a\xc3\x97"
         "3");

  // Adding different key
  buf.AppendOrIncrement("b");
  assert(buf.FullText() ==
         "a\xc3\x97"
         "3b");

  // Special key adds leading space if needed
  buf.AppendOrIncrement("Enter");
  assert(buf.FullText() ==
         "a\xc3\x97"
         "3b Enter");

  // Backspace pops "Enter"
  buf.Backspace();
  assert(buf.FullText() ==
         "a\xc3\x97"
         "3b ");

  // Backspace pops space token
  buf.Backspace();
  assert(buf.FullText() ==
         "a\xc3\x97"
         "3b");

  // Delete word
  buf.Clear();
  buf.AppendRaw("hello");
  buf.AppendRaw(" ");
  buf.AppendRaw("world");
  assert(buf.FullText() == "hello world");

  buf.DeleteWord();
  assert(buf.FullText() == "hello ");

  std::cout << "[TEST] test_buffer_operations PASSED.\n";
}

extern void TestMouseState() {
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
