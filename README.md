# TpTheme-Hypr

Qt Quick/QML replacements for the five widgets in `TpTheme-KDE/desklets`.
They use regular transparent Wayland windows and have no KDE, KWin, X11,
LayerShellQt, or Plasma dependencies. System data is read by the small
Qt backend; widget rendering and layout are QML.

## Build

On Arch Linux:

```sh
sudo pacman -S --needed cmake qt6-base qt6-declarative
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"
sudo cmake --install build
```

Run one widget:

```sh
tp-theme-hypr --widget clock
tp-theme-hypr --widget system
tp-theme-hypr --widget quote
tp-theme-hypr --widget journal
tp-theme-hypr --widget animation
```

Hyprland can keep them below normal windows and off the task switcher with
rules matching `class:^(tp-theme-hypr)$`. Position each widget with normal
Hyprland `windowrulev2` rules, for example:

```ini
windowrulev2 = float, class:^(tp-theme-hypr)$
windowrulev2 = pin, class:^(tp-theme-hypr)$
windowrulev2 = noanim, class:^(tp-theme-hypr)$
```
