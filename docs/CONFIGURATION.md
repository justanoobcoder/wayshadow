# Configuration Guide for keypop

`keypop` can be configured via a configuration file, command line flags, or a combination of both (CLI options override config file options).

---

## 1. Configuration File

The default configuration file path is:
`~/.config/keypop/keypop.conf` (or `$XDG_CONFIG_HOME/keypop/keypop.conf`).

The file follows standard INI format with a `[settings]` group.

### Example `keypop.conf`
```ini
[settings]
# Background color (Hex code #RRGGBB or #RRGGBBAA)
background = #000000

# Text foreground color (Hex code #RRGGBB or #RRGGBBAA)
foreground = #FFFFFF

# Background opacity (float from 0.0 to 1.0)
opacity = 0.6

# Font size in points (integer >= 10, default: 65)
font_size = 65

# Overlay window dimensions (WIDTHxHEIGHT in pixels)
geometry = 840x130

# Inactivity auto-hide timeout in milliseconds (0 to disable)
hide_timeout = 2000
```

---

## 2. Command Line Arguments

All settings can be specified or overridden using command line arguments:

| Flag | Argument | Description | Default |
|------|----------|-------------|---------|
| `-b` | `<color>` | Set background color (e.g. `#000000` or `000000`) | `#000000` |
| `-c` | `<color>` | Set text foreground color (e.g. `#FFFFFF`) | `#FFFFFF` |
| `-s` | `<size>` | Set font size in points | `65` |
| `-g` | `<WxH>` | Set window size in pixels | `840x130` |
| `-o` | `<opacity>` | Set background opacity (`0.0` - `1.0`) | `0.6` |
| `-t` | `<ms>` | Set auto-hide timeout in milliseconds (`0` = never) | `2000` |
| `-v` | None | Display version information and exit | - |
| `-h` | None | Display help usage and exit | - |

---

## 3. Priority Order

1. **Command line flags** (highest priority)
2. **Configuration file** (`~/.config/keypop/keypop.conf`)
3. **Hardcoded defaults** (fallback)
