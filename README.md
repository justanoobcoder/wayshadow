# keypop

A lightweight, elegant keystroke visualizer for Wayland compositors.

## Documentation

- [Architecture Guide](docs/ARCHITECTURE.md)
- [Configuration Reference](docs/CONFIGURATION.md)

## Installation

### NixOS

Add keypop input url to your `flake.nix`:
```nix
inputs = {
  nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  keypop = {
    url = "github:justanoobcoder/keypop";
    inputs.nixpkgs.follows = "nixpkgs";
  };
};
```

Add keypop to package list:
```nix
{
  pkgs,
  inputs,
  ...
}:{
  environment.systemPackages = with pkgs; [
    inputs.keypop.packages.${pkgs.stdenv.hostPlatform.system}.default
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

## License

MIT
