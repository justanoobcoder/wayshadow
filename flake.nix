{
  description = "wayshadow – a lightweight Wayland keystroke visualizer";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
    }:
    let
      overlay = final: _: {
        wayshadow = final.callPackage ./packaging/nix/package.nix { };
      };
    in
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs {
          inherit system;
          overlays = [ overlay ];
        };

        buildDeps = with pkgs; [
          cairo
          glib
          gtk3
          libappindicator-gtk3
          libinput
          libxkbcommon
          pango
          wayland
          wayland-protocols
        ];

        nativeBuildDeps = with pkgs; [
          pkg-config
          wayland-scanner
        ];
      in
      {
        packages = {
          inherit (pkgs) wayshadow;
          default = pkgs.wayshadow;
        };

        devShells.default = pkgs.mkShell {
          name = "wayshadow-dev";

          packages =
            nativeBuildDeps
            ++ buildDeps
            ++ (with pkgs; [
              gcc
              gnumake
              clang-tools
              gdb
              valgrind
              bear
              nixpkgs-fmt
            ]);

          shellHook = ''
            export WAYLAND_PROTOCOLS_DIR="${pkgs.wayland-protocols}/share/wayland-protocols"
            echo "wayshadow dev shell ready"
            echo "  make          – build the binary"
            echo "  make test     – run the test suite"
            echo "  make install  – install to PREFIX (default /usr)"
          '';
        };

        checks.default = pkgs.wayshadow;
      }
    )

    // {
      overlays.default = overlay;
      overlays.wayshadow = overlay;
    };
}
