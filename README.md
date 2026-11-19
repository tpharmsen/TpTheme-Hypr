# TpTheme-Hypr

Qt Quick/QML widgets for Hyprland. The application uses LayerShellQt for
transparent Wayland layer surfaces, a small Qt backend for system data, and
QML for widget rendering and layout.

## Build

On Arch Linux:

```sh
sudo pacman -S --needed cmake qt6-base qt6-declarative layer-shell-qt
./build.sh
```

Run one widget:

```sh
tp-theme-hypr --widget panel
tp-theme-hypr --widget clock
tp-theme-hypr --widget system
tp-theme-hypr --widget quote
tp-theme-hypr --widget journal
tp-theme-hypr --widget animation
```

Position and size widgets with `--position X,Y` and `--size WIDTHxHEIGHT`:

```sh
tp-theme-hypr --widget clock --position 120,80 --size 480x480
```

`--position` uses the top-left corner of the current screen. Without it,
widgets retain their default bottom-layer placement. `--size` overrides the
widget's default dimensions.

Hyprland resizing scales all widget content proportionally within the window.
All widgets use the `zwlr_layer_shell_v1` bottom layer through LayerShellQt.
Their exclusive zone is set to `-1`, so they do not reserve screen space.
They use the Hyprland layer namespace `tp-theme-hypr`, allowing HyprGlass
and other layer rules to match them.

The panel is a full-width bottom layer surface. It provides clickable
workspaces, live CPU/RAM/battery status, and a clock. Start it from Hyprland:

```ini
exec-once = /home/tpharmsen/Documents/TpTheme-Hypr/build/tp-theme-hypr --widget panel
windowrulev2 = noanim, class:^(tp-theme-hypr)$
```

Hyprland can keep widgets below normal windows and off the task switcher with
rules matching `class:^(tp-theme-hypr)$`:

```ini
windowrulev2 = float, class:^(tp-theme-hypr)$
windowrulev2 = pin, class:^(tp-theme-hypr)$
windowrulev2 = noanim, class:^(tp-theme-hypr)$
```
