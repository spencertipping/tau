#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>

/*
    If you get linking errors when using C++, you need
    to add extern "C" here or in X11-xcb.h, unless
    this bug is already fixed in your version:
    http://bugs.freedesktop.org/show_bug.cgi?id=22252
*/
#include <X11/Xlib-xcb.h> /* for XGetXCBConnection, link with libX11-xcb */

#include <xcb/xcb.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <xcb/xproto.h>

/*
    Attribs filter the list of FBConfigs returned by glXChooseFBConfig().
    Visual attribs further described in glXGetFBConfigAttrib(3)
*/
static int visual_attribs[] = {
    GLX_X_RENDERABLE, True,
    GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
    GLX_RENDER_TYPE, GLX_RGBA_BIT,
    GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
    GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_BLUE_SIZE, 8, GLX_ALPHA_SIZE, 8,
    GLX_DEPTH_SIZE, 24,
    GLX_STENCIL_SIZE, 8,
    GLX_DOUBLEBUFFER, True,
    // GLX_SAMPLE_BUFFERS  , 1,
    // GLX_SAMPLES         , 4,
    None};

xcb_visualid_t get_visualid_by_depth(xcb_screen_t *screen, uint16_t depth) {
    xcb_depth_iterator_t depth_iter;

    depth_iter = xcb_screen_allowed_depths_iterator(screen);
    for (; depth_iter.rem; xcb_depth_next(&depth_iter)) {
        if (depth_iter.data->depth != depth)
            continue;

        xcb_visualtype_iterator_t visual_iter;

        visual_iter = xcb_depth_visuals_iterator(depth_iter.data);
        if (!visual_iter.rem)
            continue;
        return visual_iter.data->visual_id;
    }
    return 0;
}

void draw() {
  glLoadIdentity();
  glOrtho(0, 600, 600, 0, 0.0f, 100.0f);
  glClearColor(0.2, 0.4, 0.9, 0.5);
  glClearDepth(1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  float triangle_vertex[] = {
      300, 10,  // vertex 1
      580, 550, // vertex 2
      20,  550  // vertex 3
  };
  float triangle_color[] = {
    0, 0, 0, 0.0, // red
    0, 1, 0, 1.0, // green
    0, 0, 1, 0.5  // blue
  };
  glVertexPointer(2, GL_FLOAT, 0, triangle_vertex);
  glColorPointer(4, GL_FLOAT, 0, triangle_color);
  glDrawArrays(GL_TRIANGLES, 0, 3);

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);

//  glClearColor(0.2, 0.4, 0.9, 0.5);
//  glClear(GL_COLOR_BUFFER_BIT);

  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

  float line_vertex[] = {10, 10, 400, 600};
  glVertexPointer(2, GL_FLOAT, 0, line_vertex);
  glDrawArrays(GL_LINES, 0, 2);
}

int main_loop(Display *display, xcb_connection_t *connection,
              xcb_window_t window, GLXDrawable drawable) {
  int running = 1;
  while (running) {
    /* Wait for event */
    xcb_generic_event_t *event = xcb_wait_for_event(connection);
    if (!event) {
      fprintf(stderr, "i/o error in xcb_wait_for_event");
      return -1;
    }

    switch (event->response_type & ~0x80) {
    case XCB_KEY_PRESS:
      /* Quit on key press */
//      running = 0;
      break;
    case XCB_EXPOSE:
      /* Handle expose event, draw and swap buffers */
      draw();
      glXSwapBuffers(display, drawable);
      break;
    default:
      break;
    }

    free(event);
  }

  return 0;
}

int setup_and_run(Display          *display,
                  xcb_connection_t *connection,
                  int               default_screen,
                  xcb_screen_t     *screen) {
  int visualID = 0;

  /* Query framebuffer configurations that match visual_attribs */
  GLXFBConfig *fb_configs     = 0;
  int          num_fb_configs = 0;
  fb_configs = glXChooseFBConfig(display,
                                 default_screen,
                                 visual_attribs,
                                 &num_fb_configs);

  printf("Found %d matching FB configs\n", num_fb_configs);

  /* Select first framebuffer config and query visualID */
  GLXFBConfig fb_config = fb_configs[0];
  //glXGetFBConfigAttrib(display, fb_config, GLX_VISUAL_ID, &visualID);
  visualID = get_visualid_by_depth(screen, 32);
  printf("visualID = %d\n", visualID);
  GLXContext context = glXCreateNewContext(display, fb_config, GLX_RGBA_TYPE, 0, True);

  xcb_colormap_t colormap = xcb_generate_id(connection);
  xcb_window_t   window   = xcb_generate_id(connection);
  xcb_create_colormap(connection,
                      XCB_COLORMAP_ALLOC_NONE,
                      colormap,
                      screen->root,
                      visualID);

  uint32_t eventmask = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;
  uint32_t valuelist[] =
    {screen->black_pixel,
     screen->black_pixel,
     eventmask,
     colormap,
     0};

  uint32_t valuemask =
    XCB_CW_EVENT_MASK   | XCB_CW_COLORMAP |
    XCB_CW_BORDER_PIXEL | XCB_CW_BACK_PIXEL;

  xcb_create_window(connection, 32, window, screen->root, 0,
                    0, 600, 600, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, visualID,
                    valuemask, valuelist);

  // NOTE: window must be mapped before glXMakeContextCurrent
  xcb_map_window(connection, window);

  GLXDrawable drawable  = 0;
  GLXWindow   glxwindow = glXCreateWindow(display, fb_config, window, 0);

  if (!window) {
    xcb_destroy_window(connection, window);
    glXDestroyContext(display, context);

    fprintf(stderr, "glXCreateWindow failed\n");
    return -1;
  }

  drawable = glxwindow;

  /* make OpenGL context current */
  if (!glXMakeContextCurrent(display, drawable, drawable, context)) {
    xcb_destroy_window(connection, window);
    glXDestroyContext(display, context);

    fprintf(stderr, "glXMakeContextCurrent failed\n");
    return -1;
  }

  /* run main loop */
  int retval = main_loop(display, connection, window, drawable);

  glXDestroyWindow(display, glxwindow);
  xcb_destroy_window(connection, window);
  glXDestroyContext(display, context);

  return retval;
}

int main(int argc, char *argv[]) {
  Display *display;
  int default_screen;

  /* Open Xlib Display */
  display = XOpenDisplay(0);
  default_screen = DefaultScreen(display);

  /* Get the XCB connection from the display */
  xcb_connection_t *connection = XGetXCBConnection(display);

  /* Acquire event queue ownership */
  XSetEventQueueOwner(display, XCBOwnsEventQueue);

  /* Find XCB screen */
  xcb_screen_t *screen = 0;
  xcb_screen_iterator_t screen_iter =
      xcb_setup_roots_iterator(xcb_get_setup(connection));
  for (int screen_num = default_screen; screen_iter.rem && screen_num > 0;
       --screen_num, xcb_screen_next(&screen_iter))
    ;
  screen = screen_iter.data;

  /* Initialize window and OpenGL context, run main loop and deinitialize */
  int retval = setup_and_run(display, connection, default_screen, screen);

  /* Cleanup */
  XCloseDisplay(display);

  return retval;
}
