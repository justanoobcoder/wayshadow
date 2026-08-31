{
  lib,
  stdenv,
  pkg-config,
  wayland,
  wayland-protocols,
  wayland-scanner,
  cairo,
  pango,
  libinput,
  libxkbcommon,
  gtk3,
  libappindicator-gtk3,
  glib,
}:

stdenv.mkDerivation {
  pname = "keypop";
  version = "0.1.0";

  src = ./.;

  nativeBuildInputs = [
    pkg-config
    wayland-scanner
  ];

  buildInputs = [
    wayland
    wayland-protocols
    cairo
    pango
    libinput
    libxkbcommon
    gtk3
    libappindicator-gtk3
    glib
  ];

  makeFlags = [
    "WAYLAND_PROTOCOLS_DIR=${wayland-protocols}/share/wayland-protocols"
    "GIT_COMMIT=nix-build"
  ];

  installPhase = ''
    runHook preInstall

    install -D -m 755 keypop        $out/bin/keypop
    install -D -m 644 man/keypop.1  $out/share/man/man1/keypop.1
    install -D -m 644 man/keypop.conf.5 $out/share/man/man5/keypop.conf.5

    runHook postInstall
  '';

  meta = {
    description = "Wayland keystroke visualizer";
    homepage = "https://github.com/justanoobcoder/keypop";
    license = lib.licenses.mit;
    maintainers = with lib; [ justanoobcoder ];
    platforms = lib.platforms.linux;
    mainProgram = "keypop";
  };
}
