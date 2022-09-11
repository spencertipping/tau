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

#include <pango/pangocairo.h>


#define let auto const


// https://stackoverflow.com/questions/61124564/convert-scancodes-to-ascii
char kbd_US [128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', /* <-- Tab */
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, /* <-- control key */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',  0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

char kbd_US_shift [128] =
{
  0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
  '\t', /* <-- Tab */
  'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
  0, /* <-- control key */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',  0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',   0,
  '*',
  0,  /* Alt */
  ' ',  /* Space bar */
  0,  /* Caps lock */
  0,  /* 59 - F1 key ... > */
  0,   0,   0,   0,   0,   0,   0,   0,
  0,  /* < ... F10 */
  0,  /* 69 - Num lock*/
  0,  /* Scroll Lock */
  0,  /* Home key */
  0,  /* Up Arrow */
  0,  /* Page Up */
  '-',
  0,  /* Left Arrow */
  0,
  0,  /* Right Arrow */
  '+',
  0,  /* 79 - End key*/
  0,  /* Down Arrow */
  0,  /* Page Down */
  0,  /* Insert Key */
  0,  /* Delete Key */
  0,   0,   0,
  0,  /* F11 Key */
  0,  /* F12 Key */
  0,  /* All other keys are undefined */
};


// TODO: make a library out of this file so we can include custom render/interaction
// logic and skip most of the boilerplate


// TODO: struct these up
static uint32_t w = 600;
static uint32_t h = 600;


static bool active      = true;
static bool redraw      = true;
static bool redraw_text = true;
static bool all_done    = false;


static unsigned int texture_id = 0;
static int          text_width;
static int          text_height;
static std::string  text = "ΣΦ/ϊϝ :: x² ← ∫y ∂τ";


typedef uint32_t rgba;


cairo_t*
create_layout_context ()
{
  cairo_surface_t *temp_surface;
  cairo_t *context;

  temp_surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 0, 0);
  context = cairo_create (temp_surface);
  cairo_surface_destroy (temp_surface);

  return context;
}


unsigned int
create_texture (unsigned int width,
                unsigned int height,
                unsigned char *pixels)
{
  unsigned int texture_id;

  glGenTextures (1, &texture_id);
  glBindTexture (GL_TEXTURE_2D, texture_id);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D (GL_TEXTURE_2D,
                0,
                GL_RGBA,
                width,
                height,
                0,
                GL_BGRA,
                GL_UNSIGNED_BYTE,
                pixels);

  return texture_id;
}


void
draw_texture (int width,
              int height,
              unsigned int texture_id)
{
  /* Render a texture in immediate mode. */
  glBindTexture (GL_TEXTURE_2D, texture_id);
  glColor4f (1.f, 1.f, 1.f, 1.f);

  glBegin (GL_QUADS);
  glTexCoord2f (0.0f, 0.0f);
  glVertex2f (0.0f, h * 0.5);
  glTexCoord2f (1.0f, 0.0f);
  glVertex2f (width, h * 0.5);
  glTexCoord2f (1.0f, 1.0f);
  glVertex2f (width , h * 0.5 + height);
  glTexCoord2f (0.0f, 1.0f);
  glVertex2f (0.0f, h * 0.5 + height);
  glEnd ();
}


cairo_t*
create_cairo_context (int width,
                      int height,
                      int channels,
                      cairo_surface_t** surf,
                      unsigned char** buffer)
{
  *buffer = reinterpret_cast<unsigned char*>(calloc (channels * width * height, sizeof (unsigned char)));
  *surf = cairo_image_surface_create_for_data (*buffer,
                                               CAIRO_FORMAT_ARGB32,
                                               width,
                                               height,
                                               channels * width);
  return cairo_create (*surf);
}


void
get_text_size (PangoLayout *layout,
               int         *width,
               int         *height)
{
  pango_layout_get_size(layout, width, height);
  /* Divide by pango scale to get dimensions in pixels. */
  *width /= PANGO_SCALE;
  *height /= PANGO_SCALE;
}


void
render_text (const char   *text,
             int          *text_width,
             int          *text_height,
             unsigned int *texture_id)
{
  cairo_t *layout_context;
  cairo_t *render_context;
  cairo_surface_t *surface;
  unsigned char* surface_data = NULL;
  PangoFontDescription *desc;
  PangoLayout *layout;

  if (*texture_id) glDeleteTextures(1, texture_id);

  layout_context = create_layout_context ();

  /* Create a PangoLayout, set the font and text */
  layout = pango_cairo_create_layout (layout_context);
  pango_layout_set_text (layout, text, -1);

  /* Load the font */
  desc = pango_font_description_from_string ("Gentium Italic 48");
  pango_layout_set_font_description (layout, desc);
  pango_font_description_free (desc);

  /* Get text dimensions and create a context to render to */
  get_text_size (layout, text_width, text_height);
  render_context = create_cairo_context (*text_width,
                                         *text_height,
                                         4,
                                         &surface,
                                         &surface_data);

  /* Render */
  cairo_set_source_rgba (render_context, 0.9, 0.9, 1, 1);
  pango_cairo_show_layout (render_context, layout);
  *texture_id = create_texture(*text_width, *text_height, surface_data);

  std::cout << "rendered; new texture_id = " << *texture_id << std::endl;

  /* Clean up */
  free (surface_data);
  g_object_unref (layout);
  cairo_destroy (layout_context);
  cairo_destroy (render_context);
  cairo_surface_destroy (surface);
}


xcb_visualid_t get_visualid_by_depth(xcb_screen_t *const screen,
                                     uint16_t      const depth)
{
  for (auto depth_iter = xcb_screen_allowed_depths_iterator(screen);
       depth_iter.rem;
       xcb_depth_next(&depth_iter)) {
    if (depth_iter.data->depth != depth) continue;

    let visual_iter = xcb_depth_visuals_iterator(depth_iter.data);
    if (!visual_iter.rem) continue;
    return visual_iter.data->visual_id;
  }
  return 0;
}


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
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);

  // TODO: vector of things to draw
  for (int r = 0; r < (w - 20) / 50; ++r)
    line(10 + 50*r, h / 2.,
         10 + 51*r, h / 2. + h/4. * sin(ms / 1000.0 + r / 4.),
         2 + cos(ms / 800.0 + r/3.),
         0xe0e0f0ff);

  //glClearDepth(1.0f);
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_TEXTURE_2D);
  draw_texture(text_width, text_height, texture_id);

  glXSwapBuffers(display, drawable);
  //redraw = false;
}


void draw_loop(Display     *const display,
               GLXDrawable  const drawable)
{
  using namespace std::chrono;


  std::cout << "text width = " << text_width
            << ", text height = " << text_height
            << ", texture_id = " << texture_id << std::endl;

  while (!all_done)
  {
    let start = steady_clock::now();
    let next_frame = start + 42ms;

    if (redraw)
    {
      if (redraw_text)
      {
        render_text(text.c_str(), &text_width, &text_height, &texture_id);
        redraw_text = false;
      }
      draw(display, drawable);
    }

    let micros = duration_cast<microseconds>(next_frame - steady_clock::now()).count();
    if (micros > 0) usleep(micros);
  }

  glDeleteTextures(1, &texture_id);
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

    case XCB_FOCUS_IN:
      active = true;
      redraw = true;
      break;

    case XCB_FOCUS_OUT:
      active = false;
      redraw = true;
      break;

    case XCB_KEY_PRESS: {
      let ke = reinterpret_cast<xcb_key_press_event_t*>(e);
      let k  = ke->state & XCB_MOD_MASK_SHIFT
        ? kbd_US_shift[ke->detail - 8]
        : kbd_US      [ke->detail - 8];

      if (k == 8)
      {
        if (!text.empty())
        {
          while (!text.empty() && (text[text.size() - 1] & 0xc0) == 0x80)
            text.pop_back();
          text.pop_back();
        }
      }
      else if (k)
        text.push_back(k);

      redraw = true;
      redraw_text = true;

      std::cout << "rendered text: " << text << std::endl;

      break;
    }

      /*
    case XCB_BUTTON_PRESS: {
      let pe = reinterpret_cast<xcb_button_press_event_t*>(e);
      break;
    }
      */

    case XCB_CONFIGURE_NOTIFY: {
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


int main(int argc, char *argv[])
{
  let display        = XOpenDisplay(0);
  let default_screen = DefaultScreen(display);
  let connection     = XGetXCBConnection(display);
  XSetEventQueueOwner(display, XCBOwnsEventQueue);

  auto i = xcb_setup_roots_iterator(xcb_get_setup(connection));
  for (int n = default_screen; i.rem && n > 0; --n, xcb_screen_next(&i));

  let screen = i.data;
  setup_and_run(display, connection, default_screen, screen);
  XCloseDisplay(display);
  return 0;
}
