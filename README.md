# keypop (C++ Port)

A lightweight, elegant keystroke and mouse click visualizer written in modern **C++20** for Wayland compositors.

## Features

- **Modern C++20**: RAII wrappers, strong types, `constexpr` constants, and clear separation of concerns.
- **Wayland Native**: Integrates directly with `wl_compositor`, `xdg_shell`, and `wl_shm`.
- **System-wide input monitoring**: Uses `libinput` and `udev` for low-latency keystroke and mouse tracking.
- **Repeat Compactor**: Compacts repeated keystrokes (e.g. `j×5`, `Backspace×10`).
- **Rich Vector Glyphs**: Procedural Cairo rendering for arrows, Enter, Esc, Space, and media keys.
- **Tray Indicator**: GTK3 AppIndicator with toggle show/hide and exit controls.
- **Documented & Tested**: Architecture docs, config reference, unit tests, and man pages included.

## Dependencies

| Library | Package name (Debian/Ubuntu) |
|---|---|
| Wayland client | `libwayland-dev` |
| Cairo | `libcairo2-dev` |
| Pango | `libpango1.0-dev` |
| libinput | `libinput-dev` |
| udev | `libudev-dev` |
| libxkbcommon | `libxkbcommon-dev` |
| GTK3 | `libgtk-3-dev` |
| AppIndicator3 | `libappindicator3-dev` |
| GLib | `libglib2.0-dev` |
| wayland-protocols | `wayland-protocols` |
| wayland-scanner | `wayland-scanner` |

## Building

```bash
make
```

Run the unit tests:

```bash
make test
```

Clean build artifacts:

```bash
make clean
```

## Installing

Install to the default prefix (`/usr/local`):

```bash
sudo make install
```

### Changing the installation directory

Override `PREFIX` to install elsewhere:

```bash
# Install to /usr instead of /usr/local
sudo make install PREFIX=/usr

# Install to a custom directory (no sudo needed)
make install PREFIX=$HOME/.local

# Install to a staging directory (for packaging)
make install DESTDIR=/tmp/staging
```

The individual directories can also be overridden independently:

```bash
make install BINDIR=/opt/bin MANDIR=/opt/share/man
```

| Variable | Default | Description |
|---|---|---|
| `PREFIX` | `/usr/local` | Installation root |
| `BINDIR` | `$(PREFIX)/bin` | Binary location |
| `MANDIR` | `$(PREFIX)/share/man` | Man page location |
| `DESTDIR` | *(empty)* | Staging root prefix (for packagers) |

## Uninstalling

```bash
sudo make uninstall
```

Pass the same `PREFIX`/`BINDIR`/`MANDIR` you used during install:

```bash
sudo make uninstall PREFIX=/usr
make uninstall PREFIX=$HOME/.local
```

## Documentation

- [Architecture Guide](docs/ARCHITECTURE.md)
- [Configuration Reference](docs/CONFIGURATION.md)
- [Man page: keypop(1)](man/keypop.1)
- [Man page: keypop.conf(5)](man/keypop.conf.5)

## License

MIT
