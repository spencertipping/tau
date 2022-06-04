#include <iostream>

#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h> /* for XGetXCBConnection, link with libX11-xcb */

#include <GL/gl.h>
#include <GL/glx.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>


xcb_visualid_t get_visualid_by_depth(xcb_screen_t *screen,
                                     uint16_t      depth)
{
  xcb_depth_iterator_t depth_iter;

  depth_iter = xcb_screen_allowed_depths_iterator(screen);
  for (; depth_iter.rem; xcb_depth_next(&depth_iter)) {
    if (depth_iter.data->depth != depth) continue;

    xcb_visualtype_iterator_t const visual_iter =
      xcb_depth_visuals_iterator(depth_iter.data);
    if (!visual_iter.rem) continue;
    return visual_iter.data->visual_id;
  }
  return 0;
}


void draw()
{
  glLoadIdentity();

  // TODO: query for window width/height (or track it)
  glOrtho(0, 600, 600, 0, 0.0f, 100.0f);
  glClearColor(0.06, 0.07, 0.08, 0.8);
  glClearDepth(1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  // TODO: factor this into a tau-geometry-rendering function
  float triangle_vertex[] = {
    300, 10,  // vertex 1
    580, 550, // vertex 2
    20,  550  // vertex 3
  };
  float triangle_color[] = {
    0.8, 0.8, 0.9, 0.0, // red
    0.5, 0.5, 0.5, 1.0, // green
    0.5, 0.5, 0.6, 0.5  // blue
  };

  glVertexPointer(2, GL_FLOAT, 0, triangle_vertex);
  glColorPointer(4, GL_FLOAT, 0, triangle_color);
  glDrawArrays(GL_TRIANGLES, 0, 3);

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
}


int main_loop(Display          *const display,
              xcb_connection_t *const connection,
              xcb_window_t      const window,
              GLXDrawable       const drawable)
{
  for (;;)
  {
    xcb_generic_event_t *const event = xcb_wait_for_event(connection);
    if (!event)
    {
      std::cerr << "i/o error in xcb_wait_for_event" << std::endl;
      return -1;
    }

    switch (event->response_type & ~0x80)
    {
    case XCB_EXPOSE:
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
                  xcb_screen_t     *screen)
{
  int const visual_attribs[] = {
    GLX_X_RENDERABLE, True,
    GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
    GLX_RENDER_TYPE, GLX_RGBA_BIT,
    GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
    GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_BLUE_SIZE, 8, GLX_ALPHA_SIZE, 8,
    GLX_DEPTH_SIZE, 24,
    GLX_STENCIL_SIZE, 8,
    GLX_DOUBLEBUFFER, True,
    None};

  /* Query framebuffer configurations that match visual_attribs */
  GLXFBConfig *fb_configs     = 0;
  int          num_fb_configs = 0;
  fb_configs = glXChooseFBConfig(display,
                                 default_screen,
                                 visual_attribs,
                                 &num_fb_configs);

  /* Select first framebuffer config and query visualID */
  GLXFBConfig const fb_config = fb_configs[0];
  GLXContext  const context   = glXCreateNewContext(display,
                                                    fb_config,
                                                    GLX_RGBA_TYPE,
                                                    0,
                                                    True);

  xcb_colormap_t const colormap = xcb_generate_id(connection);
  xcb_window_t   const window   = xcb_generate_id(connection);
  int            const visualID = get_visualid_by_depth(screen, 32);
  xcb_create_colormap(connection,
                      XCB_COLORMAP_ALLOC_NONE,
                      colormap,
                      screen->root,
                      visualID);

  uint32_t valuelist[] = {screen->black_pixel,
                          screen->black_pixel,
                          XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS,
                          colormap,
                          0};

  uint32_t valuemask = XCB_CW_EVENT_MASK   | XCB_CW_COLORMAP |
                       XCB_CW_BORDER_PIXEL | XCB_CW_BACK_PIXEL;

  xcb_create_window(connection,
                    32,
                    window,
                    screen->root,
                    0, 0, 600, 600,
                    0,
                    XCB_WINDOW_CLASS_INPUT_OUTPUT,
                    visualID,
                    valuemask,
                    valuelist);

  // NOTE: window must be mapped before glXMakeContextCurrent
  xcb_map_window(connection, window);

  GLXWindow const glxwindow = glXCreateWindow(display, fb_config, window, 0);
  glXMakeContextCurrent(display, glxwindow, glxwindow, context);
  int       const r         = main_loop(display, connection, window, glxwindow);

  glXDestroyWindow(display, glxwindow);
  xcb_destroy_window(connection, window);
  glXDestroyContext(display, context);
  return r;
}

int main(int argc, char *argv[])
{
  Display          *const display        = XOpenDisplay(0);
  int               const default_screen = DefaultScreen(display);
  xcb_connection_t *const connection     = XGetXCBConnection(display);
  XSetEventQueueOwner(display, XCBOwnsEventQueue);

  xcb_screen_t          *screen = 0;
  xcb_screen_iterator_t  screen_iter =
      xcb_setup_roots_iterator(xcb_get_setup(connection));
  for (int screen_num = default_screen; screen_iter.rem && screen_num > 0;
       --screen_num, xcb_screen_next(&screen_iter));
  screen = screen_iter.data;

  int const r = setup_and_run(display, connection, default_screen, screen);
  XCloseDisplay(display);
  return r;
}
