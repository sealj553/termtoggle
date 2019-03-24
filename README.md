# termtoggle

If the terminal is running, this program will toggle its visibility. If the terminal is not running, it will be started. This is designed to be bound to a key (I use the backtick/tilde key) for ease of use.

This works just like tilde or guake/yakuake, except for any arbitrary terminal. I have it set up for [st](https://st.suckless.org/), but it's easily changed.

main.c:
```c
const char *target_name = "dropdown";
const char *start_cmd   = "st -t dropdown &";
```
This runs st with the title "dropdown" (doesn't matter, just needs to be unique), then will look for a window with that title.

My openbox setup (rc.xml) for this includes:
```xml
    <application title="dropdown">
      <decor>no</decor>
      <desktop>all</desktop>
      <skip_pager>yes</skip_pager>
      <skip_taskbar>yes</skip_taskbar>
      <maximized>yes</maximized>
      <focus>yes</focus>
    </application>
```

This removes window decorations and removes the window from the taskbar.
