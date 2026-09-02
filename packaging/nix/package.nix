{
  lib,
  stdenv,
  cairo,
  glib,
  gtk3,
  libappindicator-gtk3,
  libinput,
  libxkbcommon,
  pango,
  pkg-config,
  wayland,
  wayland-protocols,
  wayland-scanner,
}:

stdenv.mkDerivation {
  pname = "wayshadow";
  version = "1.0.0";

  src = ../..;

  nativeBuildInputs = [
    pkg-config
    wayland-scanner
  ];

  buildInputs = [
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

  makeFlags = [
    "WAYLAND_PROTOCOLS_DIR=${wayland-protocols}/share/wayland-protocols"
    "GIT_COMMIT=nix-build"
  ];

  installPhase = ''
    runHook preInstall

    install -D -m 755 wayshadow        $out/bin/wayshadow
    install -D -m 644 man/wayshadow.1  $out/share/man/man1/wayshadow.1
    install -D -m 644 man/wayshadow.conf.5 $out/share/man/man5/wayshadow.conf.5
    install -D -m 644 public/way_shadow.svg $out/share/wayshadow/way_shadow.svg
    install -D -m 644 public/way_shadow.svg $out/share/icons/hicolor/scalable/apps/way_shadow.svg

    runHook postInstall
  '';

  meta = {
    description = "Wayland keystroke visualizer";
    homepage = "https://github.com/justanoobcoder/wayshadow";
    license = lib.licenses.mit;
    maintainers = with lib; [ justanoobcoder ];
    platforms = lib.platforms.linux;
    mainProgram = "wayshadow";
  };
}
