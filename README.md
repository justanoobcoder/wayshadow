# wayshadow

A lightweight, elegant keystroke visualizer for Wayland compositors.

WayShadow is a C++ rewrite/port of [keypop](https://github.com/yossefsabry/keypop) by [Yossef Sabry](https://github.com/yossefsabry).

## Documentation

- [Architecture Guide](docs/ARCHITECTURE.md)
- [Configuration Reference](docs/CONFIGURATION.md)

## Installation

### NixOS

Add wayshadow input url to your `flake.nix`:
```nix
inputs = {
  nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  wayshadow = {
    url = "github:justanoobcoder/wayshadow";
    inputs.nixpkgs.follows = "nixpkgs";
  };
};
```

Add wayshadow to package list:
```nix
{
  pkgs,
  inputs,
  ...
}:{
  environment.systemPackages = with pkgs; [
    inputs.wayshadow.packages.${pkgs.stdenv.hostPlatform.system}.default
  ];
}
```

Add your user to `input` group:
```nix
users.users.YOUR_USERNAME = {
  extraGroups = [
    "wheel"
    "input"
  ];
};
```

## Building

Ensure you have the required development libraries installed (`wayland-scanner`, `wayland-protocols`, `cairo`, `pango`, `libinput`, `libudev`, `libxkbcommon`, `gtk-3`, `libappindicator-gtk3`).

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

## How to use

Wayshadow requires access to input devices, so add your user to the `input` group (NixOS users can skip this since the above configuration already added your user to `input` group):

```bash
sudo usermod -aG input $USER
# Log out and back in
```

Then run:
```bash
wayshadow
```

Or run with custom options:
```bash
# E.g., Blue background, Red text, Size 80, 1000x200 window, 80% opacity, hide after 5 seconds
wayshadow -b "#0000FF" -c "#FF0000" -s 80 -g 1000x200 -o 0.8 -t 5000
```

Options:
- `-b <color>`: Background color hex (e.g. `#000000` or `000000`)
- `-c <color>`: Text color hex (e.g. `#FFFFFF` or `FFFFFF`)
- `-s <size>`: Font size (default 65)
- `-g <WxH>`: Window geometry (default 840x130)
- `-o <opacity>`: Background opacity (0.0 - 1.0, default 0.6)
- `-t <time>`: Window hide time in milliseconds (0 = never hide, default: 2000)
- `-v`: Get version info
- `-h`: Show help

## Configuration

Wayshadow can be configured via config file. Create a file named `wayshadow.conf` in `$XDG_CONFIG_HOME/wayshadow/` and add the following:

```
[settings]
background=#000000
foreground=#FFFFFF
font_size=65
geometry=840x130
opacity=0.6
hide_timeout=2000
```

## License

MIT
