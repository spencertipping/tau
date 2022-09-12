#ifndef τξlinux_xframe_gltext_h
#define τξlinux_xframe_gltext_h


#include <GL/gl.h>
#include <GL/glx.h>

#include <cairo.h>
#include <pango/pangocairo.h>
#include <pango/pango-font.h>
#include <pango/pango-layout.h>


#include "../begin.hh"

namespace τ::ξ
{


typedef u32        rgba;
typedef rgba const rgbac;


struct gl_text
{
  St     f;
  St     t;
  rgba   c;
  GLuint tid;
  int    w;
  int    h;

  gl_text() {}
  gl_text(St f_, St t_, rgba c_) : f(f_), t(t_), c(c_)
  { cairo_surface_t *cs;
    let cl = cairo_create(cs = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 0, 0));
    cairo_surface_destroy(cs);

    let pl = pango_cairo_create_layout(cl);
    let pd = pango_font_description_from_string(f.c_str());
    pango_layout_set_font_description(pl, pd);
    pango_font_description_free(pd);

    pango_layout_get_size(pl, &w, &h);
    w /= PANGO_SCALE;
    h /= PANGO_SCALE;

    let b  = calloc(w * h * 4, 1);
    let cr = cairo_create(cs = cairo_image_surface_create_for_data(Rc<uch*>(b), CAIRO_FORMAT_ARGB32, w, h, w * 4));

    cairo_set_source_rgba(cr,
                          (c >> 24)        / 255.,
                          (c >> 16 & 0xff) / 255.,
                          (c >> 8  & 0xff) / 255.,
                          (c       & 0xff) / 255.);
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

  void x() { glDeleteTextures(1, &tid); }

  void r(f32 x, f32 y, f32 sw = 1.f, f32 sh = 1.f) const
    { glBindTexture(GL_TEXTURE_2D, tid);
      glColor4f(1.f, 1.f, 1.f, 1.f);
      glBegin(GL_QUADS);
      glTexCoord2f(0.f, 0.f); glVertex2f(x,        y);
      glTexCoord2f(1.f, 0.f); glVertex2f(x + sw*w, y);
      glTexCoord2f(1.f, 1.f); glVertex2f(x + sw*w, y + sh*h);
      glTexCoord2f(0.f, 1.f); glVertex2f(x,        y + sh*h);
      glEnd(); }

  uN size() const { return w * h * 4; }
};


}

#include "../end.hh"


#endif
