# Architecture of keypop (C++ Port)

This document describes the architectural design, component interactions, and data flow of **keypop** (C++20 port).

## 1. High-Level Overview

`keypop` is a minimalist, elegant keystroke and mouse click visualization overlay designed for Wayland compositors. It intercepts input events system-wide using `libinput` via `udev` and renders smooth, configurable visual indicators onto a Wayland ARGB surface with `cairo` and `pango`.

```
                +---------------------+
                |     libinput /      |
                |     evdev (udev)    |
                +----------+----------+
                           |
                           v
+------------------+  InputManager  +------------------+
|   TrayIcon       |       |        |   WaylandContext |
|   (AppIndicator) |       v        |   (wl_compositor,|
+--------+---------+  XkbHandler    |    xdg_wm_base,  |
         |                 |        |    wl_shm)       |
         |                 v        +--------+---------+
         |            TextBuffer             |
         |                 |                 |
         +--------\        v        /--------+
                   \  Application  /
                    \      |      /
                     v     v     v
                     WindowManager
                           |
                           v
                        Renderer
                           |
                           v
                     Cairo / wl_shm
```

---

## 2. Core Subsystems & Components

### 2.1 `keypop::core` (`color`, `config`, `buffer`, `state`)
- **`Color`**: Strong value-type representation of RGBA values with `constexpr` palette constants and hex parser (`#RRGGBB` / `#RRGGBBAA`).
- **`Config`**: Strong configuration settings loaded from `~/.config/keypop/keypop.conf` or command-line arguments.
- **`TextBuffer`**: Manages the visible keystroke stream, history, repeat compaction (e.g. `k×4`), atomic backspaces, and word deletions.
- **`ClientState`**: Encapsulates runtime application state, modifier keys status, mouse position, click timestamps, and redraw dirty flags.

### 2.2 `keypop::input` & `keypop::xkb` (`input`, `xkb_handler`)
- **`InputManager`**: Opens `libinput` devices via `udev` under `seat0`. Dispatches events into high-level C++ callbacks.
- **`XkbHandler`**: RAII wrapper around `xkb_context`, `xkb_keymap`, and `xkb_state`. Translates Linux input scancodes into UTF-8 characters, special symbol names (`Enter`, `Esc`, `Tab`), and modifier combos (`Ctrl+Shift+T`).

### 2.3 `keypop::render` (`draw`, `icons`, `shm`)
- **`ShmPool`**: POSIX shared memory file creator (`shm_open`, `ftruncate`, `wl_shm_create_pool`).
- **`Icons`**: Procedural vector drawing for keyboard glyphs (Arrows, Enter, Media controls, Function key badges) rendered directly into Cairo contexts.
- **`Renderer`**: Computes text measurements, right-aligned scrolling offsets, superscripts for repeat multipliers, and mouse coordinate badges.

### 2.4 `keypop::wayland` & `keypop::ui` (`wl_setup`, `window`, `tray`, `app`)
- **`WaylandContext`**: Manages Wayland client connection (`wl_display`), registry listener, compositor, seat, and `xdg_wm_base`.
- **`WindowManager`**: Manages creation, configuration, destruction, and presentation of `xdg_surface` and `xdg_toplevel`.
- **`TrayIcon`**: Integrates with Desktop Application Status Tray (`libappindicator` and `gtk3`) to toggle overlay visibility or exit the application.
- **`Application`**: Glues GLib main event loop (`GMainLoop`), Wayland socket file descriptors, input event channel, and frame timer (16ms / ~60 FPS).
