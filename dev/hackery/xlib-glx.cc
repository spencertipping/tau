#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>

#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h> /* for XGetXCBConnection, link with libX11-xcb */

#include <GL/gl.h>
#include <GL/glx.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>


// TODO: struct these up
static uint32_t w = 600;
static uint32_t h = 600;


static bool redraw   = true;
static bool all_done = false;


xcb_visualid_t get_visualid_by_depth(xcb_screen_t *const screen,
                                     uint16_t      const depth)
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


// TODO: arc()


void line(double const x1, double const y1,
          double const x2, double const y2,
          double const w,
          double const r, double const g,
          double const b, double const a)
{
  // TODO: convert to endpoints + cross-width so we can do diagonals
  glBegin(GL_TRIANGLE_FAN);
  glColor4f(r, g, b, a);

  double u   = y2 - y1;
  double v   = x1 - x2;
  double rho = w * 0.5 / sqrt(u*u + v*v);
  u *= rho;
  v *= rho;

  glVertex2d(x1-u, y1-v);
  glVertex2f(x1+u, y1+v);
  glVertex2f(x2+u, y2+v);
  glVertex2f(x2-u, y2-v);
  glEnd();
}


void draw(Display     *const display,
          GLXDrawable  const drawable)
{
  using namespace std::chrono;

  static uint32_t last_w = 0;
  static uint32_t last_h = 0;

  float ms = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();

  if (last_w != w || last_h != h) glViewport(0, 0, last_w = w, last_h = h);

  glLoadIdentity();
  glOrtho(0, w, h, 0, 0.0f, 100.0f);
  glClearColor(0.06, 0.07, 0.08, 0.8);
  glClearDepth(1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_MULTISAMPLE);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  for (int r = 0; r < (w - 20) / 50; ++r)
    line(10 + 50*r, h / 2.,
         10 + 51*r, h / 2. + h/4. * sin(ms / 1000.0 + r / 4.),
         20,
         0.8, 0.8, 0.9, 1.0);

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);

  glXSwapBuffers(display, drawable);
  //redraw = false;
}


void draw_loop(Display     *const display,
               GLXDrawable  const drawable)
{
  using namespace std::chrono;

  while (!all_done)
  {
    time_point<steady_clock> const start      = steady_clock::now();
    time_point<steady_clock> const next_frame = start + 20ms;

    if (redraw) draw(display, drawable);

    int const micros = duration_cast<microseconds>(next_frame - steady_clock::now()).count();
    if (micros > 0) usleep(micros);
  }
}


int main_loop(Display          *const display,
              xcb_connection_t *const connection,
              xcb_window_t      const window,
              GLXDrawable       const drawable)
{
  for (xcb_generic_event_t *e;
       (e = xcb_wait_for_event(connection));
       free(e))
  {
    switch (e->response_type & ~0x80)
    {
    case XCB_EXPOSE:
      redraw = true;
      break;

    case XCB_CONFIGURE_NOTIFY: {
      xcb_configure_notify_event_t *const ne =
        (xcb_configure_notify_event_t*) e;

      if (ne->width > 0 && ne->height > 0 && (ne->width != w || ne->height != h))
      {
        w = ne->width;
        h = ne->height;
        redraw  = true;
      }

      break;
    }

    default:
      break;
    }
  }

  all_done = true;

  return 0;
}


void setup_and_run(Display          *const display,
                   xcb_connection_t *const connection,
                   int               const default_screen,
                   xcb_screen_t     *const screen)
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
    GLX_SAMPLE_BUFFERS, 1,   // for MSAA
    GLX_SAMPLES, 8,          // for MSAA
    None};

  GLXFBConfig *fb_configs     = 0;
  int          num_fb_configs = 0;
  fb_configs = glXChooseFBConfig(display,
                                 default_screen,
                                 visual_attribs,
                                 &num_fb_configs);

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

  uint32_t const valuelist[] =
    {screen->black_pixel,
     screen->black_pixel,
     XCB_EVENT_MASK_EXPOSURE
       | XCB_EVENT_MASK_KEY_PRESS
       | XCB_EVENT_MASK_STRUCTURE_NOTIFY,
     colormap,
     0};

  xcb_create_window(connection,
                    32,
                    window,
                    screen->root,
                    0, 0, w, h,
                    0,
                    XCB_WINDOW_CLASS_INPUT_OUTPUT,
                    visualID,
                    XCB_CW_EVENT_MASK
                      | XCB_CW_COLORMAP
                      | XCB_CW_BORDER_PIXEL
                      | XCB_CW_BACK_PIXEL,
                    valuelist);

  // NOTE: window must be mapped before glXMakeContextCurrent
  xcb_map_window(connection, window);

  GLXWindow const glxwindow = glXCreateWindow(display, fb_config, window, 0);
  glXMakeContextCurrent(display, glxwindow, glxwindow, context);

  std::thread event_thread(main_loop, display, connection, window, glxwindow);
  draw_loop(display, glxwindow);
  event_thread.join();

  glXDestroyWindow(display, glxwindow);
  xcb_destroy_window(connection, window);
  glXDestroyContext(display, context);
}


int main(int argc, char *argv[])
{
  Display          *const display        = XOpenDisplay(0);
  int               const default_screen = DefaultScreen(display);
  xcb_connection_t *const connection     = XGetXCBConnection(display);
  XSetEventQueueOwner(display, XCBOwnsEventQueue);

  xcb_screen_iterator_t i = xcb_setup_roots_iterator(xcb_get_setup(connection));
  for (int n = default_screen; i.rem && n > 0; --n, xcb_screen_next(&i));

  xcb_screen_t *const screen = i.data;
  setup_and_run(display, connection, default_screen, screen);
  XCloseDisplay(display);
  return 0;
}
