#if τgl
#include "gl.hh"
#include "../begin.hh"

namespace σ
{

using namespace τ;


GLuint gl_text::texture()
{
  if (tid) return tid;

  int w, h;
  cairo_surface_t *cs;
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

  cairo_set_source_rgba(cr, bg.r, bg.g, bg.b, bg.a);
  cairo_paint(cr);
  cairo_set_source_rgba(cr, fg.r, fg.g, fg.b, fg.a);
  pango_cairo_show_layout(cr, pl);

  glGenTextures(1, &tid);
  glBindTexture(GL_TEXTURE_2D, tid);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
               GL_BGRA, GL_UNSIGNED_BYTE, b);

  free(b);
  g_object_unref(pl);
  cairo_destroy(cl);
  cairo_destroy(cr);
  cairo_surface_destroy(cs);

  return tid;
}


vec2 gl_text::measure()
{
  int w, h;
  cairo_surface_t *cs;
  let cl = cairo_create(cs = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 0, 0));
  cairo_surface_destroy(cs);

  let pl = pango_cairo_create_layout(cl);
  let pd = pango_font_description_from_string(f.c_str());
  pango_layout_set_text(pl, t.data(), t.size());
  pango_layout_set_font_description(pl, pd);
  pango_layout_get_size(pl, &w, &h);
  pango_font_description_free(pd);
  dims = {f64(w) / PANGO_SCALE, f64(h) / PANGO_SCALE};
  g_object_unref(pl);
  cairo_destroy(cl);

  return dims;
}


}

#include "../end.hh"
#endif
