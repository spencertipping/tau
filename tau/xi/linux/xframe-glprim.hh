#ifndef τξlinux_xframe_glprim_h
#define τξlinux_xframe_glprim_h


#include <cmath>

#include <GL/gl.h>
#include <GL/glx.h>


#include "../begin.hh"

namespace τ::ξ
{


typedef u32        rgba;
typedef rgba const rgbac;


inline f32 rgba_r(rgba x) { return (x >> 24)        / 255.f; }
inline f32 rgba_g(rgba x) { return (x >> 16 & 0xff) / 255.f; }
inline f32 rgba_b(rgba x) { return (x >>  8 & 0xff) / 255.f; }
inline f32 rgba_a(rgba x) { return (x       & 0xff) / 255.f; }


void gl_line(f32c x1, f32c y1,
             f32c x2, f32c y2, f32c w, rgbac c)
{
  glEnable(GL_MULTISAMPLE);
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);

  glBegin(GL_TRIANGLE_FAN);
  glColor4f(rgba_r(c), rgba_g(c), rgba_b(c), rgba_a(c));

  f32  u = y2 - y1;
  f32  v = x1 - x2;
  f32c ρ = w * 0.5 / sqrt(u*u + v*v);
  u *= ρ;
  v *= ρ;

  glVertex2f(x1-u, y1-v);
  glVertex2f(x1+u, y1+v);
  glVertex2f(x2+u, y2+v);
  glVertex2f(x2-u, y2-v);
  glEnd();
}


}

#include "../end.hh"


#endif
