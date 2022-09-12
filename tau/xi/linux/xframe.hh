#ifndef τξlinux_xframe_h
#define τξlinux_xframe_h


#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/Xlib-xcb.h>
#include <X11/Xlib.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>


#include "../basics.hh"
#include "../digamma.hh"

#include "xframe-glprim.hh"
#include "xframe-gltext.hh"
#include "xframe-keymap.hh"
#include "xframe-xcbio.hh"
#include "xframe-x11.hh"

#include "../begin.hh"

namespace τ::ξ
{


struct xframe_
{
  sletc fa = 0x1;          // flag: frame is active

  Φ        &f;
  uN        w  = 0;
  uN        h  = 0;
  uf8       fs = 0;
  uf8       b;             // number of bits for rb and ob
  ζ<i9>    *rb;            // render buffer
  ζ<i9>    *ob;            // offscreen buffer
  gl_texts  tc;
  x11gl     gl;
  rgba      bgf;           // background when focused
  rgba      bgu;           // background when unfocused

  xframe_(Φ   &f_,
          uf8  b_  = 20,
          chc *dp_ = nullptr,
          uN   w_  = Sc<uN>(720 * nφ),
          uN   h_  = 720)
    : f(f_), b(b_),
      rb{new ζ<i9>(f.l, b_)},
      ob{new ζ<i9>(f.l, b_)},
      gl{w_, h_, 0, 0, dp_}
    {}

  xframe_ &render()
    { gl.clear(fs & fa ? bgf : bgu);

    }
};


ϝ &xframe(Φ &f)
{
  let   x = new xframe_(f);
  auto &e = xcb_events(f, x->gl.c) | tee(f);

  e | sink<i9>(f, [&, x](i9 y)
    { if (y.type() == u9t::nstruct)
      { let e = Rc<xcb_generic_event_t*>(y.data());
        switch (e->response_type & ~0x80)
        {
        case XCB_EXPOSE:                           x->render(); break;
        case XCB_FOCUS_IN:  x->fs |=  xframe_::fa; x->render(); break;
        case XCB_FOCUS_OUT: x->fs &= ~xframe_::fa; x->render(); break;
        case XCB_CONFIGURE_NOTIFY:
        { let ne = Rc<xcb_configure_notify_event_t*>(e);
          if (ne->width > 0 && ne->height > 0)
          { x->w = ne->width;
            x->h = ne->height;
            x->render(); }
          break; }
        }}});

  // TODO: how to turn e into ϝ's output? maybe just copy through

  auto u = [&, x](ϝ &f) {};

  auto &r = (new ϝ(f, ϝ::ξΦ, u))->xf([x](ϝ&) { delete x; });

  return r;
}


}

#include "../end.hh"


#endif
