# keypop

A lightweight, elegant keystroke visualizer for Wayland compositors.

## Building

Ensure you have the required development libraries installed (`wayland-client`, `wayland-protocols`, `cairo`, `pango`, `libinput`, `libudev`, `libxkbcommon`, `gtk-3`, `libappindicator-gtk3`).

```bash
# Build the binary
make

# Run the test suite
make test

# Install binary and man pages
sudo make install

# Uninstall :(
sudo make uninstall
```

## Documentation

- [Architecture Guide](docs/ARCHITECTURE.md)
- [Configuration Reference](docs/CONFIGURATION.md)

## License

MIT
