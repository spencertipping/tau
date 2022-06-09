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


#define let auto const


// TODO: make a library out of this file so we can include custom render/interaction
// logic and skip most of the boilerplate


// TODO: struct these up
static uint32_t w = 600;
static uint32_t h = 600;


static bool active   = true;
static bool redraw   = true;
static bool all_done = false;


typedef uint32_t rgba;


inline
xcb_visualid_t get_visualid_by_depth(xcb_screen_t *const screen,
                                     uint16_t      const depth)
{
  for (auto depth_iter = xcb_screen_allowed_depths_iterator(screen);
       depth_iter.rem;
       xcb_depth_next(&depth_iter))
  {
    if (depth_iter.data->depth != depth) continue;

    let visual_iter = xcb_depth_visuals_iterator(depth_iter.data);
    if (!visual_iter.rem) continue;
    return visual_iter.data->visual_id;
  }
  return 0;
}


inline
void line(double const x1, double const y1,
          double const x2, double const y2,
          double const w,
          rgba   const rgba)
{
  glBegin(GL_TRIANGLE_FAN);
  glColor4f((rgba >> 24       ) / 255.,
            (rgba >> 16 & 0xff) / 255.,
            (rgba >> 8  & 0xff) / 255.,
            (rgba       & 0xff) / 255.);

  double       u   = y2 - y1;
  double       v   = x1 - x2;
  double const rho = w * 0.5 / sqrt(u*u + v*v);
  u *= rho;
  v *= rho;

  glVertex2d(x1-u, y1-v);
  glVertex2d(x1+u, y1+v);
  glVertex2d(x2+u, y2+v);
  glVertex2d(x2-u, y2-v);
  glEnd();
}


inline
void draw(Display     *const display,
          GLXDrawable  const drawable)
{
  using namespace std::chrono;

  static uint32_t last_w = 0;
  static uint32_t last_h = 0;

  let ms = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();

  if (last_w != w || last_h != h) glViewport(0, 0, last_w = w, last_h = h);

  glLoadIdentity();
  glOrtho(0, w, h, 0, 0.0f, 100.0f);
  glClearColor(0.01, 0.02, 0.03, active ? 0.8 : 0.4);
  glClearDepth(1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_MULTISAMPLE);

  // TODO: vector of things to draw
  for (int r = 0; r < (w - 20) / 50; ++r)
    line(10 + 50*r, h / 2.,
         10 + 51*r, h / 2. + h/4. * sin(ms / 1000.0 + r / 4.),
         2 + cos(ms / 800.0 + r/3.),
         0xe0e0f0ff);

  glXSwapBuffers(display, drawable);
  //redraw = false;
}


inline
void draw_loop(Display     *const display,
               GLXDrawable  const drawable)
{
  using namespace std::chrono;

  while (!all_done)
  {
    let start = steady_clock::now();
    let next_frame = start + 42ms;  // 24fps

    if (redraw) draw(display, drawable);

    let micros = duration_cast<microseconds>(next_frame - steady_clock::now()).count();
    if (micros > 0) usleep(micros);
  }
}


inline
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

    case XCB_FOCUS_IN:
      active = true;
      redraw = true;
      break;

    case XCB_FOCUS_OUT:
      active = false;
      redraw = true;
      break;

      /*
    case XCB_BUTTON_PRESS:
    {
      let pe = reinterpret_cast<xcb_button_press_event_t*>(e);
      break;
    }
      */

    case XCB_CONFIGURE_NOTIFY:
    {
      let ne = reinterpret_cast<xcb_configure_notify_event_t*>(e);
      if (ne->width > 0 && ne->height > 0 && (ne->width != w || ne->height != h))
      {
        w = ne->width;
        h = ne->height;
        redraw = true;
      }
      break;
    }

    default:
      std::cout << "unhandled event of type " << (int) e->response_type << std::endl;
      break;
    }
  }

  all_done = true;

  return 0;
}


inline
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

  let fb_config = fb_configs[0];
  let context   = glXCreateNewContext(display,
                                      fb_config,
                                      GLX_RGBA_TYPE,
                                      0,
                                      True);

  let colormap = xcb_generate_id(connection);
  let window   = xcb_generate_id(connection);
  let visualID = get_visualid_by_depth(screen, 32);
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
       | XCB_EVENT_MASK_KEY_RELEASE
       | XCB_EVENT_MASK_POINTER_MOTION
       | XCB_EVENT_MASK_BUTTON_MOTION
       | XCB_EVENT_MASK_BUTTON_PRESS
       | XCB_EVENT_MASK_BUTTON_RELEASE
       | XCB_EVENT_MASK_FOCUS_CHANGE
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

  let glxwindow = glXCreateWindow(display, fb_config, window, 0);
  glXMakeContextCurrent(display, glxwindow, glxwindow, context);

  std::thread event_thread(main_loop, display, connection, window, glxwindow);
  draw_loop(display, glxwindow);
  event_thread.join();

  glXDestroyWindow(display, glxwindow);
  xcb_destroy_window(connection, window);
  glXDestroyContext(display, context);
}


inline
void frame(char const *const dpy = NULL)
{
  let display        = XOpenDisplay(dpy);
  let default_screen = DefaultScreen(display);
  let connection     = XGetXCBConnection(display);
  XSetEventQueueOwner(display, XCBOwnsEventQueue);

  auto i = xcb_setup_roots_iterator(xcb_get_setup(connection));
  for (int n = default_screen; i.rem && n > 0; --n, xcb_screen_next(&i));

  let screen = i.data;
  setup_and_run(display, connection, default_screen, screen);
  XCloseDisplay(display);
}
