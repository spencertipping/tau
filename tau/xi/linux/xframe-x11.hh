#ifndef τξlinux_xframe_x11_h
#define τξlinux_xframe_x11_h


#include <X11/Xlib-xcb.h>
#include <X11/Xlib.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

#include <GL/gl.h>
#include <GL/glx.h>


#include "xframe-glprim.hh"

#include "../begin.hh"

namespace τ::ξ
{


xcb_visualid_t get_visualid_by_depth(xcb_screen_t *const s,
                                     u16c                d)
{
  for (auto i = xcb_screen_allowed_depths_iterator(s);
       i.rem;
       xcb_depth_next(&i))
  {
    if (i.data->depth != d) continue;
    let j = xcb_depth_visuals_iterator(i.data);
    if (!j.rem) continue;
    return j.data->visual_id;
  }
  return 0;
}


struct x11gl
{
  sletc evs0 = XCB_EVENT_MASK_EXPOSURE
    | XCB_EVENT_MASK_KEY_PRESS
    | XCB_EVENT_MASK_POINTER_MOTION
    | XCB_EVENT_MASK_BUTTON_MOTION
    | XCB_EVENT_MASK_BUTTON_PRESS
    | XCB_EVENT_MASK_BUTTON_RELEASE
    | XCB_EVENT_MASK_FOCUS_CHANGE
    | XCB_EVENT_MASK_STRUCTURE_NOTIFY;

  static int const constexpr vas0[] =
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
    GLX_SAMPLES,        8,  // for MSAA
    None};


  Display          *dp;
  xcb_connection_t *c;
  xcb_screen_t     *s;
  GLXContext        glc;
  GLXWindow         glw;
  u32               wid;
  int               si;  // default screen index
  uN                w;
  uN                h;

  x11gl(uN w_, uN h_, uN x_ = 0, uN y_ = 0,
        chc       *dp_ = nullptr,
        u32        evs = evs0,
        int const *vas = vas0) : w(w_), h(h_)

    { dp = XOpenDisplay(dp_);
      si = DefaultScreen(dp);
      c  = XGetXCBConnection(dp);
      XSetEventQueueOwner(dp, XCBOwnsEventQueue);

      auto i = xcb_setup_roots_iterator(xcb_get_setup(c));
      for (int n = si; i.rem && n > 0; --n, xcb_screen_next(&i));
      s = i.data;

      int nfbcs = 0;
      let fbcs  = glXChooseFBConfig(dp, si, vas, &nfbcs);
      A(nfbcs > 0, "no matching X11 FBconfigs (i.e. we cannot map a GL window; try changing vas)");

      let fbc   = fbcs[0];
          glc   = glXCreateNewContext(dp, fbc, GLX_RGBA_TYPE, 0, True);
      let cm    = xcb_generate_id(c);
          wid   = xcb_generate_id(c);
      let vid   = get_visualid_by_depth(s, 32);
      xcb_create_colormap(c, XCB_COLORMAP_ALLOC_NONE, cm, s->root, vid);

      u32c vl[] = { s->black_pixel, s->black_pixel, evs0, cm, 0 };
      xcb_create_window(
        c, 32, wid, s->root,
        x_, y_, w, h,
        0, XCB_WINDOW_CLASS_INPUT_OUTPUT, vid,
        XCB_CW_EVENT_MASK | XCB_CW_COLORMAP | XCB_CW_BORDER_PIXEL | XCB_CW_BACK_PIXEL,
        vl);

      // NOTE: window must be mapped before glXMakeContextCurrent
      xcb_map_window(c, wid);

      glw = glXCreateWindow(dp, fbc, wid, 0);
      glXMakeContextCurrent(dp, glw, glw, glc); }

  ~x11gl()
    { glXDestroyWindow(dp, glw);
      xcb_destroy_window(c, wid);
      glXDestroyContext(dp, glc);
      XCloseDisplay(dp); }

  x11gl &clear(rgba bg)
    { glLoadIdentity();
      glOrtho(0, w, h, 0, 0.0f, 100.0f);
      glClearColor(rgba_r(bg), rgba_g(bg), rgba_b(bg), rgba_a(bg));
      glClearDepth(1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      return *this; }

  x11gl &swap() { glXSwapBuffers(dp, glw); return *this; }
};


}

#include "../end.hh"


#endif
