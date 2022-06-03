#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

// Window size
int height = 256, width = 256;

XImage *CreateTrueColorImage(Display *display, Visual *visual)
{

    int i, j;
    unsigned char *image32=(unsigned char *)malloc(width*height*4);
    unsigned char *p=image32;
    for(i=0; i<width; i++)
    {
        for(j=0; j<height;j++)
        {
            *p++ = i;
            *p++ = i;
            *p++ = j;
            *p++ = j; // alpha channel (should progressively get transparent towards left)
        }
    }

    // Replacing "DefaultDepth(display,DefaultScreen(display))" with a hardcoded
    // 24 or 32 still doesn't work with XCreateWindow. XCreateSimpleWindow works
    // with hardcoded 24, but not 32.
    return XCreateImage(display, visual, 32, ZPixmap, 0, image32, width, height, 32, 0);
}

int main(int argc, char **argv)
{
    XImage *ximage;
    Display *display = XOpenDisplay(NULL);
    Visual *visual = DefaultVisual(display, 0);

    XVisualInfo vinfo;
    XMatchVisualInfo(display, DefaultScreen(display), 32, TrueColor, &vinfo);

    XSetWindowAttributes attr;
    attr.colormap = XCreateColormap(display, DefaultRootWindow(display),
            vinfo.visual, AllocNone);
    attr.border_pixel = 0;
    attr.background_pixel = 0x80800000; // Red, semi-transparent

    //Window window = XCreateSimpleWindow(display, RootWindow(display, 0), 0, 0,
    //      width, height, 1, 0, 0);
    Window window = XCreateWindow(display, DefaultRootWindow(display), 0, 0,
            width, height, 0, vinfo.depth, InputOutput, vinfo.visual,
            CWColormap | CWBorderPixel | CWBackPixel, &attr);
    GC gc = XCreateGC(display, window, 0, 0);

    ximage = CreateTrueColorImage(display, vinfo.visual);
    XSelectInput(display, window, ButtonPressMask|ExposureMask);
    XMapWindow(display, window);

    while(1)
    {
        XEvent event;
        XNextEvent(display, &event);
        switch(event.type)
        {
        case Expose:
            XPutImage(display, window, gc, ximage,
                    0, 0, 0, 0, width, height);
            break;
        case ButtonPress:
            exit(0);
        }
    }
}
