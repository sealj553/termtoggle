# termtoggle

If the terminal is running, this program will toggle its visibility. If the terminal is not running, it will be started. This is designed to be bound to a key (I like the backtick key) for ease of use.

This works just like tilde or guake/yakuake, except for any arbitrary terminal.
I have it set up for st, but it's easily changed.

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
