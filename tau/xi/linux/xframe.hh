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


enum xframe_rop
{
  xrop_text,  // (font, text, c, x, y)
  xrop_line,  // (x1, y1, z1, x2, y2, z2, z, w, c)
};


struct xframe_
{
  sletc fa = 0x1;          // flag: frame is active

  Φ        &f;
  Display  *dp;
  ΣΘΔ       rh;
  uf8       fs = 0;
  uf8       b;             // number of bits for rb and ob
  ζ<i9>    *rb;            // render buffer
  ζ<i9>    *ob;            // offscreen buffer
  gl_texts  tc;
  x11gl     gl;
  rgba      bgf;           // background when focused
  rgba      bgu;           // background when unfocused

  xframe_(Φ       &f_,
          Display *d_,
          uf8      b_   = 20,
          uN       w_   = Sc<uN>(720 * nφ),
          uN       h_   = 720,
          rgba     bgf_ = 0x020407e0,
          rgba     bgu_ = 0x02040780)
    : f(f_), dp(d_), b(b_),
      rb{new ζ<i9>(f.l, b_)},
      ob{new ζ<i9>(f.l, b_)},
      gl{d_, w_, h_, 0, 0},
      bgf{bgf_}, bgu{bgu_} {}

  xframe_ &render_one(i9 x)
    { switch (Sc<uN>(x[0]))
      {
      case xrop_text: tc.r   (x[1], x[2], x[3], x[4], x[5], x[6]);             break;
      case xrop_line: gl_line(x[1], x[2], x[3], x[4], x[5], x[6], x[7], x[8]); break;
      default: A(0, "unsupported rop " << x);
      }
      return *this; }

  xframe_ &resize(uN w, uN h)
    { glViewport(0, 0, gl.w = w, gl.h = h);
      return render(); }

  xframe_ &render()
    { rh.start();
      gl.begin();
      gl.clear(fs & fa ? bgf : bgu);
      for (uN i = 0; i < rb->b.ra(); i += i9::size_of(*rb + i))
        render_one(*rb + i);
      gl.end();
      tc.gc();
      rh.stop();
      return *this; }

  xframe_ &operator<<(i9 x)
    { if (x.type() == u9t::stream && Sc<u9st>(x) == u9st::τ)
      { let z = rb; rb = ob; (ob = z)->b.reset(); render(); }
      else A(*ob << o9(x), "xframe_ << overflow; received " << x);
      return *this; }

  xcb_connection_t *c() const { return gl.c; }
};


ϝ &xframe(Φ &f)
{
  let   d = XOpenDisplay(nullptr);
  let   x = new xframe_(f, d);
  auto &e = xcb_events(f, x->c()) | tee(f);

  // TODO: demultiplex events by which window they belong to
  // otherwise we can't create multiple frames

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
          if (ne->width > 0 && ne->height > 0) x->resize(ne->width, ne->height);
          break; }
        }}});

  return e ^ (new ϝ(f, ϝ::ξΦ | 0x020000,
                    [&, x](ϝ &f) { for (let a : f) *x << a; },
                    [&]   (ϝ &f) { f.α() < f.δ(); }))
    ->xf([x](ϝ&) { delete x; });
}


}

#include "../end.hh"


#endif
