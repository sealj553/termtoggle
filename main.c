#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXSTR 1000

Display *display;
unsigned char *prop;
unsigned long window;

const char *target_name = "dropdown";
const char *start_cmd   = "st -t dropdown &";

static char* get_property(Display *disp, Window win, Atom xa_prop_type, char *prop_name, unsigned long *size){
    Atom xa_prop_name, xa_ret_type;
    int ret_format;
    unsigned long ret_nitems, ret_bytes_after;
    unsigned char *ret_prop;

    xa_prop_name = XInternAtom(disp, prop_name, False);

    if(XGetWindowProperty(disp, win, xa_prop_name, 0, MAXSTR/ 4, False,
                xa_prop_type, &xa_ret_type, &ret_format,     
                &ret_nitems, &ret_bytes_after, &ret_prop) != Success){
        fprintf(stderr, "Cannot get %s property.\n", prop_name);
        return NULL;
    }

    if(xa_ret_type != xa_prop_type){
        fprintf(stderr, "Invalid type of %s property.\n", prop_name);
        XFree(ret_prop);
        return NULL;
    }

    //null terminate the result to make string handling easier
    unsigned long tmp_size = (ret_format / 8) * ret_nitems;
    char *ret = malloc(tmp_size + 1);
    memcpy(ret, ret_prop, tmp_size);
    ret[tmp_size] = '\0';

    if(size){ *size = tmp_size; }

    XFree(ret_prop);
    return ret;
}

Window* winlist(unsigned long *len){
    Atom type, prop = XInternAtom(display,"_NET_CLIENT_LIST", False);
    int form;
    unsigned long remain;
    unsigned char *list;

    if(XGetWindowProperty(display, XDefaultRootWindow(display), prop, 0, 1024, False, XA_WINDOW, &type, &form, len, &remain, &list) != Success){
        perror("winlist() -- GetWinProp");
        return 0;
    }

    return (Window*)list;
}

int winstreq(Window win, const char *name){
    char *net_wm_name;
    if(!(net_wm_name = get_property(display, win, XInternAtom(display, "UTF8_STRING", False), "_NET_WM_NAME", NULL))){
        return 0;
    }

    int retval = !strcmp(net_wm_name, name);
    free(net_wm_name);
    return retval;
}

int error_handler(Display *disp, XErrorEvent *err){ return 0; }

int main(int argc, char** argv){
    display = XOpenDisplay(NULL);
    if(!display){ fprintf(stderr, "%s:  unable to open display '%s'\n", argv[0], XDisplayName(NULL)); }

    XSetErrorHandler(error_handler); //prevents BadMatch when window is already focused

    Window focused_window;
    int revert;
    XGetInputFocus(display, &focused_window, &revert);

    unsigned long num_windows;
    Window *windows = winlist(&num_windows);
    for(unsigned long i = 0; i < num_windows; ++i){
        //if the window exists...
        if(!winstreq(windows[i], target_name)) continue;

        //and is the focused window...
        if(windows[i] == focused_window){
            //minimize it
            XIconifyWindow(display, windows[i], 0);
        } else {
            //raise and focus it
            XClientMessageEvent ev;
            memset (&ev, 0, sizeof(ev));
            ev.type = ClientMessage;
            ev.window = windows[i];
            ev.message_type = XInternAtom(display, "_NET_ACTIVE_WINDOW", True);
            ev.format = 32;
            ev.data.l[0] = 1;
            ev.data.l[1] = CurrentTime;
            ev.data.l[2] = ev.data.l[3] = ev.data.l[4] = 0;
            XSendEvent(display, RootWindow(display, XDefaultScreen(display)), False, SubstructureRedirectMask |SubstructureNotifyMask, (XEvent*)&ev);
        }
        goto exit;
    }

    //if we got here, the window doesn't exist, so start it
    system(start_cmd);

exit:
    XFlush(display);
    XSync(display, 0);
    XCloseDisplay(display);

    return 0;
}
