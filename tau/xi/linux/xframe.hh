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

#include "cairo.h"
#include "pango/pango-font.h"
#include "pango/pangocairo.h"
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
  xrop_text,  // (font, text, c, x, y, z)
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
      case xrop_text: tc.r   (x[1], x[2], Sc<rgba>(x[3]), Sc<f32>(x[4]), Sc<f32>(x[5]), Sc<f32>(x[6]));             break;
      case xrop_line: gl_line(Sc<f32>(x[1]), Sc<f32>(x[2]), Sc<f32>(x[3]),
                              Sc<f32>(x[4]), Sc<f32>(x[5]), Sc<f32>(x[6]),
                              Sc<f32>(x[7]), Sc<rgba>(x[8])); break;
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
    { if (x.τ())
      { let z = rb; rb = ob; (ob = z)->b.reset(); render(); }
      else A(*ob << o9(x), "xframe_ << overflow; received " << x);
      return *this; }

  xcb_connection_t *c() const { return gl.c; }
};


// (font, text) -> (font, text, w, h), where (w, h) are pixel values
// TODO: standardize the behavior of functional operators like this
// -- for example, does it make sense to allow other args for keying?
// should the caller zip the output?
ϝ &text_size(Φ &f)
{
  return *new ϝ(f, "text_size", ϝ::ξι, [](ϝ &f)
    { int w, h;
      let cs = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 0, 0);
      let cl = cairo_create(cs);
      cairo_surface_destroy(cs);
      for (let x : f.α())
        if (x.type() != u9t::tuple) f.β() << x;
        else
        { let pl = pango_cairo_create_layout(cl);
          let pd = pango_font_description_from_string(Sc<St>(x[0]).c_str());
          pango_layout_set_font_description(pl, pd);
          pango_layout_set_text(pl, Rc<chc*>(x[1].data()), x[1].size());
          pango_layout_get_size(pl, &w, &h);
          pango_font_description_free(pd);
          g_object_unref(pl);
          if (!(f.β() << o9t(x[0], x[1], Sc<f32>(w) / PANGO_SCALE, Sc<f32>(h) / PANGO_SCALE)))
            break; }
      cairo_destroy(cl); });
}


ϝ &xframe(Φ &f)
{
  let   d = XOpenDisplay(nullptr);  // TODO: reuse this?
  let   x = new xframe_(f, d);
  auto &e = xcb_events(f, x->c()) | tee(f);

  // TODO: demultiplex events by which window they belong to
  // otherwise we can't create multiple frames
  //
  // TODO: unpack these structs to UTF9 maps for easier use

  e | sink<i9>(f, [x](i9 y)
    { if (y.is_istruct())
      { let e = Sc<xcb_generic_event_t*>(y);
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

  return e ^ (new ϝ(f, "xframe", ϝ::ξΦ | 0x020000,
                    [&, x](ϝ &f) { for (let a : f) *x << a; },
                    [&]   (ϝ &f) { f.α() < f.δ(); }))
    ->xf([x](ϝ&) { delete x; });
}


}

#include "../end.hh"


#endif
