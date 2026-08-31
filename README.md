# keypop (C++ Port)

A lightweight, elegant keystroke and mouse click visualizer written in modern **C++20** for Wayland compositors.

## Features

- **Modern C++20**: Clean RAII wrappers, separation of concerns, strong types, and `constexpr` where possible.
- **Wayland Native**: Integrates directly with `wl_compositor`, `xdg_shell`, and `wl_shm`.
- **System-wide input monitoring**: Utilizes `libinput` and `udev` for low-latency keystroke and mouse tracking.
- **Repeat Compactor**: Compacts repeated key strokes (e.g. `j×5`, `Backspace×10`).
- **Rich Vector Glyphs**: High-fidelity procedural rendering for arrows, Enter, Esc, Space, and media keys.
- **Tray Indicator**: GTK3 AppIndicator with toggle show/hide and exit controls.
- **Comprehensive Docs & Tests**: Includes architectural documentation, config guides, unit tests, and manual pages.

## Building

Ensure you have the required development libraries installed (`wayland-client`, `cairo`, `pango`, `libinput`, `libudev`, `libxkbcommon`, `gtk+-3.0`, `appindicator3-0.1`).

```bash
# Build the binary
make

# Run the test suite
make test

# Install binary and man pages
sudo make install
```

## Documentation

- [Architecture Guide](docs/ARCHITECTURE.md)
- [Configuration Reference](docs/CONFIGURATION.md)
- [Manual Page (keypop.1)](man/keypop.1)
- [Manual Page (keypop.conf.5)](man/keypop.conf.5)

## License

MIT
