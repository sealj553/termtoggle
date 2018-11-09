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

/*
unsigned char* get_string_property(char* property_name){
    Atom actual_type;
    int actual_format;
    unsigned long nitems, bytes_after;

    Atom filter_atom = XInternAtom(display, property_name, True);
    int status = XGetWindowProperty(display, window, filter_atom, 0, MAXSTR, False, AnyPropertyType, &actual_type, &actual_format, &nitems, &bytes_after, &prop);

    if(status == BadWindow){
        printf("window id # 0x%lx does not exists!", window);
        exit(1);
    }

    if(status != Success){
        printf("XGetWindowProperty failed!");
        exit(2);
    }
    return prop;
}

unsigned long get_current_window_prop(void){
    int screen = XDefaultScreen(display);
    window = RootWindow(display, screen);
    get_string_property("_NET_ACTIVE_WINDOW");
    unsigned long long_property = prop[0] + (prop[1]<<8) + (prop[2]<<16) + (prop[3]<<24);
    window = long_property;
    return window;
}
*/

static char *get_property(Display *disp, Window win, Atom xa_prop_type, char *prop_name, unsigned long *size){
    Atom xa_prop_name, xa_ret_type;
    int ret_format;
    unsigned long ret_nitems, ret_bytes_after;
    unsigned char *ret_prop;

    xa_prop_name = XInternAtom(disp, prop_name, False);

    //MAX_PROPERTY_VALUE_LEN / 4 explanation (XGetWindowProperty manpage):
    //long_length = Specifies the length in 32-bit multiples of the data to be retrieved.
    if (XGetWindowProperty(disp, win, xa_prop_name, 0, MAXSTR/ 4, False,
                xa_prop_type, &xa_ret_type, &ret_format,     
                &ret_nitems, &ret_bytes_after, &ret_prop) != Success) {
        fprintf(stderr, "Cannot get %s property.\n", prop_name);
        return NULL;
    }

    if (xa_ret_type != xa_prop_type) {
        fprintf(stderr, "Invalid type of %s property.\n", prop_name);
        XFree(ret_prop);
        return NULL;
    }

    //null terminate the result to make string handling easier
    unsigned long tmp_size = (ret_format / 8) * ret_nitems;
    char *ret = malloc(tmp_size + 1);
    memcpy(ret, ret_prop, tmp_size);
    ret[tmp_size] = '\0';

    if (size) {
        *size = tmp_size;
    }

    XFree(ret_prop);
    return ret;
}

Window *winlist(unsigned long *len){
    Atom prop = XInternAtom(display,"_NET_CLIENT_LIST", False), type;
    int form;
    unsigned long remain;
    unsigned char *list;

    if(XGetWindowProperty(display,XDefaultRootWindow(display),prop,0,1024,False,XA_WINDOW,&type,&form,len,&remain,&list) != Success){
        perror("winlist() -- GetWinProp");
        return 0;
    }

    return (Window*)list;
}

int window_name_is(Window win, const char *name){
    char *net_wm_name;
    if(!(net_wm_name = get_property(display, win, XInternAtom(display, "UTF8_STRING", False), "_NET_WM_NAME", NULL))){
        return 0;
    }

    if(strcmp(net_wm_name, name) == 0){
        free(net_wm_name);
        return 1;
    } else {
        free(net_wm_name);
        return 0;
    }
}

//int window_name_is_2(Window win, const char *name){
//    char *net_wm_name;
//
//    if(!(net_wm_name = get_property(display, win, XA_STRING, "_NET_WM_NAME", NULL))){
//        return 0;
//    }
//
//    if(strcmp(net_wm_name, name) == 0){
//        free(net_wm_name);
//        return 1;
//    } else {
//        free(net_wm_name);
//        return 0;
//    }
//}

int main(int argc, char** argv){
    display = XOpenDisplay(NULL);
    if(display == NULL){
        fprintf (stderr, "%s:  unable to open display '%s'\n", argv[0], XDisplayName(NULL));
    }

    Window focused_window;
    int revert;
    XGetInputFocus(display, &focused_window, &revert);

    unsigned long num_windows;
    Window *windows = winlist(&num_windows);
    for(unsigned long i = 0; i < num_windows; ++i){

        //if the window exists...
        if(window_name_is(windows[i], target_name)){
            puts("window exists");

            //and is the focused window...
            if(focused_window == windows[i]){
                puts("and is focused");

                //minimize it
                goto exit;
            } else {
                puts("and is not focused");

                //focus it
                goto exit;
            }
        }
    }

    //if we got here, the window doesn't exist, so start it
    puts("window not found, starting thing");
    //system(start_cmd);

exit:
    XCloseDisplay(display);
    return 0;
}
