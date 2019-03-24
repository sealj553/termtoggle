#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXSTR 1000
#define _NET_WM_STATE_REMOVE        0    /* remove/unset property */
#define _NET_WM_STATE_ADD           1    /* add/set property */
#define _NET_WM_STATE_TOGGLE        2    /* toggle property  */

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

static int client_msg(Display *disp, Window win, char *msg,
        unsigned long data0, unsigned long data1, 
        unsigned long data2, unsigned long data3,
        unsigned long data4) {
    XEvent event;
    long mask = SubstructureRedirectMask | SubstructureNotifyMask;

    event.xclient.type          = ClientMessage;
    event.xclient.serial        = 0;
    event.xclient.send_event    = True;
    event.xclient.message_type  = XInternAtom(disp, msg, False);
    event.xclient.window        = win;
    event.xclient.format        = 32;
    event.xclient.data.l[0]     = data0;
    event.xclient.data.l[1]     = data1;
    event.xclient.data.l[2]     = data2;
    event.xclient.data.l[3]     = data3;
    event.xclient.data.l[4]     = data4;

    if(XSendEvent(disp, DefaultRootWindow(disp), False, mask, &event)){
        return EXIT_SUCCESS;
    } else {
        fprintf(stderr, "Cannot send %s event.\n", msg);
        return EXIT_FAILURE;
    }
}

int error_handler(Display *disp, XErrorEvent *err){ return 0; }

int main(int argc, char** argv){
    display = XOpenDisplay(NULL);
    if(!display){ fprintf(stderr, "%s:  unable to open display '%s'\n", argv[0], XDisplayName(NULL)); }

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
            Atom prop1 = XInternAtom(display, "_NET_WM_STATE_HIDDEN", False);
            client_msg(display, focused_window, "_NET_WM_STATE", _NET_WM_STATE_ADD, (unsigned long)prop1, 0, 0, 0);
        } else {
            //raise and focus it
            XMapRaised(display, windows[i]);
            XSetErrorHandler(error_handler); //prevents BadMatch when window is already focused
            XSetInputFocus(display, windows[i], RevertToNone, CurrentTime);
        }
        goto exit;
    }

    //if we got here, the window doesn't exist, so start it
    system(start_cmd);

exit:
    XCloseDisplay(display);
    return 0;
}
