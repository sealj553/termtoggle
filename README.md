# termtoggle

If the terminal is running, this program will toggle its visibility. If the terminal is not running, it will be started with `start_cmd`. This is designed to be bound to a key (I use the backtick/tilde key) for ease of use. This probably only works for X11 on Linux.

This works just like tilde or guake/yakuake, except for any arbitrary terminal. You need the window title to be unique, ideally something you can set with command-line arguments.

main.c:
```c
const char *target_name = "dropdown";
```
if you use [st](https://st.suckless.org/):
```c
const char *start_cmd   = "st -t dropdown &";
```
or if you use [alacritty](https://github.com/alacritty/alacritty):
```c
const char *start_cmd   = "alacritty -t dropdown &";
```

TODO:
* ~~make it work properly with GNOME~~
* optionally take `target_name` and `start_cmd` through command-line arguments
* does this still work on Wayland??
