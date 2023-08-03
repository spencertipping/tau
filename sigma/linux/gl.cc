#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <X11/Xlib.h>

#include "gl.hh"

#include "../begin.hh"

namespace σ
{


slet Γgl_ = Ψauto([](Stc &display, ψ q, ξi i, ξo o)
  {
    x11_gl_window   w(display);
    gl_render_state rs(&w);

    std::cerr << "created the window" << std::endl;

    // TODO: add event listening

    q.pin();

    for (let &x : i)
    {
      std::cerr << "received render insn " << x << std::endl;
      if (x.is_s())
        if      (x.cs() == "b") rs.begin();
        else if (x.cs() == "e") rs.end();
        else if (x.cs() == "r") rs.run(), w.swap();
        else                    std::cerr << "unknown wop: " << x << std::endl;
      else
        rs << x.η();
    }

    std::cerr << "received render insns" << std::endl;

    while (1)
    {
      std::cerr << "output loop" << std::endl;
      q.t().Θ(now() + 500ms);
      o.r() << "rendering";
      rs.run();
      w.swap();
    }
  });


Γ Γgl_window(Stc &display, Ψd d)
{
  return ΓΨ(Γgl_(display), d);
}


void Γlinux_gl(Γφ &g)
{
  g.def_p2("gl", Γgl_);
}


static xcb_visualid_t get_visualid_by_depth(xcb_screen_t *const s,
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


x11_gl_window::x11_gl_window(Stc       &display,
                             vec2c     &pos,
                             vec2c     &dims,
                             colorc    &bgf,
                             colorc    &bgu,
                             u32        evs,
                             int const *vas)
  : gl_window_base(display),
    gl_usable(),
    dims_(dims),
    bgf_(bgf),
    bgu_(bgu)
{
  dp_ = XOpenDisplay(display.c_str());
  si_ = DefaultScreen(dp_);
  c_  = XGetXCBConnection(dp_);
  XSetEventQueueOwner(dp_, XCBOwnsEventQueue);

  auto i = xcb_setup_roots_iterator(xcb_get_setup(c_));
  for (int n = si_; i.rem && n > 0; --n, xcb_screen_next(&i));
  s_ = i.data;

  int nfbcs = 0;
  let fbcs  = glXChooseFBConfig(dp_, si_, vas, &nfbcs);
  A(nfbcs > 0, "no matching X11 FBconfigs (i.e. we cannot map a GL window; try changing vas)");

  let fbc   = fbcs[0];
      glc_  = glXCreateNewContext(dp_, fbc, GLX_RGBA_TYPE, 0, True);
  let cm    = xcb_generate_id(c_);
      wid_  = xcb_generate_id(c_);
  let vid   = get_visualid_by_depth(s_, 32);
  xcb_create_colormap(c_, XCB_COLORMAP_ALLOC_NONE, cm, s_->root, vid);

  u32c vl[] = { s_->black_pixel, s_->black_pixel, evs0, cm, 0 };
  xcb_create_window(
    c_, 32, wid_, s_->root,
    pos.x, pos.y, dims.x, dims.y,
    0, XCB_WINDOW_CLASS_INPUT_OUTPUT, vid,
    XCB_CW_EVENT_MASK | XCB_CW_COLORMAP | XCB_CW_BORDER_PIXEL | XCB_CW_BACK_PIXEL,
    vl);

  // NOTE: window must be mapped before glXMakeContextCurrent
  xcb_map_window(c_, wid_);
  glw_ = glXCreateWindow(dp_, fbc, wid_, 0);
}


x11_gl_window::~x11_gl_window()
{
  glXDestroyWindow(dp_, glw_);
  xcb_destroy_window(c_, wid_);
  glXDestroyContext(dp_, glc_);
  xcb_disconnect(c_);
  XCloseDisplay(dp_);
}


void x11_gl_window::clear(colorc &bg)
{
  let w = dims_.x;
  let h = dims_.y;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(w * -0.5, w * 0.5, h * 0.5, h * -0.5, 1., 100.);
  //glFrustum(w * -1., w, h, h * -1., 1., 100.);
  //glOrtho(0, w, h, 0, 0.0, 100.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glClearColor(bg.r, bg.g, bg.b, bg.a);
  glClearDepth(1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


fd_t x11_gl_window::xcb_fd()
{
  return xcb_get_file_descriptor(c_);
}


xcb_generic_event_t *x11_gl_window::xcb_poll()
{
  return xcb_poll_for_event(c_);
}


void x11_gl_window::use()
{
  glXMakeContextCurrent(dp_, glw_, glw_, glc_);
}


void x11_gl_window::swap()
{
  glXSwapBuffers(dp_, glw_);
}


}

#include "../end.hh"
