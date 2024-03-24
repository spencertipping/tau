#ifndef σlinux_gl_h
#define σlinux_gl_h

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <GL/glx.h>

#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xproto.h>

#include <pango/pangocairo.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>

#include "../gl.hh"
#include "../begin.hh"

namespace σ
{


struct x11_gl_window final : public virtual gl_window_base,
                             public virtual gl_usable
{
  sletc evs0 = XCB_EVENT_MASK_EXPOSURE
    | XCB_EVENT_MASK_KEY_PRESS
    | XCB_EVENT_MASK_KEY_RELEASE
    | XCB_EVENT_MASK_POINTER_MOTION
    | XCB_EVENT_MASK_BUTTON_MOTION
    | XCB_EVENT_MASK_BUTTON_PRESS
    | XCB_EVENT_MASK_BUTTON_RELEASE
    | XCB_EVENT_MASK_FOCUS_CHANGE
    | XCB_EVENT_MASK_STRUCTURE_NOTIFY;

  sc int const vas0[] =
  { GLX_X_RENDERABLE,   True,
    GLX_DRAWABLE_TYPE,  GLX_WINDOW_BIT,
    GLX_RENDER_TYPE,    GLX_RGBA_BIT,
    GLX_X_VISUAL_TYPE,  GLX_TRUE_COLOR,
    GLX_RED_SIZE,       8,
    GLX_GREEN_SIZE,     8,
    GLX_BLUE_SIZE,      8,
    GLX_ALPHA_SIZE,     8,
    GLX_DEPTH_SIZE,     24,
    GLX_STENCIL_SIZE,   8,
    GLX_DOUBLEBUFFER,   True,
    GLX_SAMPLE_BUFFERS, 1,  // for MSAA
    GLX_SAMPLES,        4,  // for MSAA
    None };


  Display           *dp_;
  xcb_connection_t  *c_;
  xcb_screen_t      *s_;
  xcb_key_symbols_t *ks_;
  GLXContext         glc_;
  GLXWindow          glw_;
  τ::u32             wid_;
  int                si_;    // screen index
  vec2               pos_;
  vec2               dims_;
  color              bgf_;
  color              bgu_;

  x11_gl_window(Stc       &display,
                vec2c     &pos  = { 200, 200 },
                vec2c     &dims = { 1080, 640 },
                colorc    &bgf  = { 0.1, 0.2, 0.3, 0.9 },
                colorc    &bgu  = { 0.1, 0.2, 0.3, 0.5 },
                τ::u32     evs  = evs0,
                int const *vas  = vas0);

  ~x11_gl_window();

  void clear(colorc &bg);

  τ::fd_t              xcb_fd();
  xcb_generic_event_t *xcb_poll();
  char                 xcb_decode_keycode(xcb_keycode_t, bool);

  void notify_resize(vec2c &dims);

  void use()  override;
  void swap() override;
  vec2 dims() override { return dims_; }
};


void Γlinux_gl(Γφ&);


}

#include "../end.hh"

#endif
