#include "gl.hh"

#include "../begin.hh"
#include <GL/glx.h>

namespace σ
{


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


static void glxc_summarize(Display *d, GLXFBConfig *c, ξo const &o)
{
  static M<St, int> as
    { {"use_gl", GLX_USE_GL},
      {"buffer_size", GLX_BUFFER_SIZE},
      {"glx_level", GLX_LEVEL},
      {"double_buffer", GLX_DOUBLEBUFFER},
      {"stereo", GLX_STEREO},
      {"aux_buffers", GLX_AUX_BUFFERS},
      {"red_size", GLX_RED_SIZE},
      {"green_size", GLX_GREEN_SIZE},
      {"blue_size", GLX_BLUE_SIZE},
      {"alpha_size", GLX_ALPHA_SIZE},
      {"depth_size", GLX_DEPTH_SIZE},
      {"stencil_size", GLX_STENCIL_SIZE},
      {"accum_red_size", GLX_ACCUM_RED_SIZE},
      {"accum_green_size", GLX_ACCUM_GREEN_SIZE},
      {"accum_blue_size", GLX_ACCUM_BLUE_SIZE},
      {"accum_alpha_size", GLX_ACCUM_ALPHA_SIZE},
      {"sample_buffers", GLX_SAMPLE_BUFFERS},
      {"samples", GLX_SAMPLES} };

  auto r = o.r();
  int v;
  for (let &[k, a] : as)
  {
    glXGetFBConfigAttrib(d, *c, a, &v);
    r << ηname{k} << v;
  }
}


slet Γglcs_ = Ψauto([](Stc &display, ξo o)
  {
    let dp    = XOpenDisplay(display.c_str());
    let si    = DefaultScreen(dp);
    int nfbcs = 0;
    let fbcs  = glXChooseFBConfig(dp, si, 0, &nfbcs);
    A(nfbcs > 0, "no FBconfigs found");

    for (int i = 0; i < nfbcs; ++i)
      glxc_summarize(dp, &fbcs[i], o);

    XCloseDisplay(dp);
  });


slet Γgl_ = Ψauto([](Stc &display, ψ q, ξi i, ξo o)
  {
    St              d = display.empty() ? getenv("DISPLAY") : display;
    x11_gl_window   w(d);
    gl_render_state rs(&w);

    let xfd = w.xcb_fd();
    let xc  = w.c_;

    A(q.t().reg(xfd, true, false), "failed to register xcb FD");
    q.fx([=](ψ_ &q_) { q_.t().unreg(xfd); });
    q.f([=, &t=q.t()]()
      { while (t.rg(xfd)->y(λs::τR))
          for (xcb_generic_event_t *e;
               e = xcb_poll_for_event(xc);)
            if (!o) return;
            else    o.r() << Stv{Rc<chc*>(e), sizeof(*e)}; });

    for (let &x : i)
      if (x.is_s())
        if      (x.cs() == "b") rs.begin();
        else if (x.cs() == "e") rs.end();
        else if (x.cs() == "r") rs.run(), w.swap();
        else                    A(0, "unknown rop: " << x);
      else
        rs << x.η();
  });


Γ Γgl_window(Stc &display, Ψd d)
{
  return ΓΨ(Γgl_(display), d);
}


void Γlinux_gl(Γφ &g)
{
  g .def_p1("glcs", Γglcs_)
    .def_p2("gl",   Γgl_);
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
  use();
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
