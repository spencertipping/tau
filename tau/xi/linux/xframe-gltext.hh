#ifndef τξlinux_xframe_gltext_h
#define τξlinux_xframe_gltext_h


#include <GL/gl.h>
#include <GL/glx.h>

#include <cairo.h>
#include <pango/pangocairo.h>
#include <pango/pango-font.h>
#include <pango/pango-layout.h>


#define XXH_INLINE_ALL
#include "../../../dep/xxhash.h"

#include "xframe-glprim.hh"


#include "../begin.hh"

namespace τ::ξ
{


inline u64 gl_text_key(Stc &f, Stc &t, rgba c)
{
  return XXH64(f.data(), f.size(), XXH64(t.data(), t.size(), c));
}


struct gl_text
{
  St     f;
  St     t;
  rgba   c;
  GLuint tid;
  int    w;
  int    h;
  uf8    fs;  // flags for GC purposes

  gl_text() {}
  gl_text(St f_, St t_, rgba c_) : f(f_), t(t_), c(c_)
  { cairo_surface_t *cs;
    let cl = cairo_create(cs = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 0, 0));
    cairo_surface_destroy(cs);

    let pl = pango_cairo_create_layout(cl);
    let pd = pango_font_description_from_string(f.c_str());
    pango_layout_set_text(pl, t.data(), t.size());
    pango_layout_set_font_description(pl, pd);
    pango_layout_get_size(pl, &w, &h);
    pango_font_description_free(pd);
    w /= PANGO_SCALE;
    h /= PANGO_SCALE;

    let b  = calloc(w * h * 4, 1);
    let cr = cairo_create(cs = cairo_image_surface_create_for_data(Rc<uch*>(b), CAIRO_FORMAT_ARGB32, w, h, w * 4));

    cairo_set_source_rgba(cr, rgba_r(c), rgba_g(c), rgba_b(c), rgba_a(c));
    pango_cairo_show_layout(cr, pl);

    glGenTextures(1, &tid);
    glBindTexture(GL_TEXTURE_2D, tid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, b);

    free(b);
    g_object_unref(pl);
    cairo_destroy(cl);
    cairo_destroy(cr);
    cairo_surface_destroy(cs); }

  gl_text &x()       { glDeleteTextures(1, &tid); tid = 0; return *this; }
  u64      k() const { return gl_text_key(f, t, c); }

  gl_text &r(f32 x, f32 y, f32 z = 1.f, f32 sw = 1.f, f32 sh = 1.f)
    { glEnable(GL_BLEND);
      //glDisable(GL_DEPTH_TEST);
      //glEnable(GL_ALPHA_TEST);
      glEnable(GL_TEXTURE_2D);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glBindTexture(GL_TEXTURE_2D, tid);
      glColor4f(1.f, 1.f, 1.f, 1.f);
      glBegin(GL_QUADS);
      glTexCoord2f(0.f, 0.f); glVertex3f(x,        y,        -z);
      glTexCoord2f(1.f, 0.f); glVertex3f(x + sw*w, y,        -z);
      glTexCoord2f(1.f, 1.f); glVertex3f(x + sw*w, y + sh*h, -z);
      glTexCoord2f(0.f, 1.f); glVertex3f(x,        y + sh*h, -z);
      glEnd();
      return *this; }

  uN size() const { return w * h * 4; }
};


// A GL-texture text cache, so you can just render stuff without
// worrying about storage
struct gl_texts
{
  sletc fm = 0x1;  // a gl_text is marked as being in-use

  M<u64, gl_text> ts;
  V<u64>          dc;  // deletion list

  ~gl_texts() { for (auto &[_, t] : ts) t.x(); }

  gl_texts &r(Stc &f, Stc &t, rgba c, f32 x, f32 y, f32 z = 1.f, f32 sw = 1.f, f32 sh = 1.f)
    { let k = gl_text_key(f, t, c);
      if (!ts.contains(k)) ts[k] = gl_text(f, t, c);
      ts[k].r(x, y, z, sw, sh).fs |= fm;
      return *this; }

  gl_texts &gc()
    { for (auto &[k, t] : ts)
      { if (!(t.fs & fm)) dc.push_back(k);
        t.fs &= ~fm; }
      for (let k : dc) { ts[k].x(); ts.erase(k); }
      dc.clear();
      return *this; }

  uN size() const
    { uN r = 0;
      for (let &[_, t] : ts) r += t.size();
      return r; }
};


}

#include "../end.hh"


#endif
