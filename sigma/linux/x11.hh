#ifndef σlinux_x11_h
#define σlinux_x11_h

#include <cairo/cairo.h>
#include <pango/pangocairo.h>
#include <pango/pango-font.h>
#include <pango/pango-layout.h>

#include "../../sigma.hh"
#include "../begin.hh"

namespace σ
{

using namespace τ;


typedef u32 rgba_t;

ic f32 rgba_r(rgba_t x) { return (x >> 24)        / 255.f; }
ic f32 rgba_g(rgba_t x) { return (x >> 16 & 0xff) / 255.f; }
ic f32 rgba_b(rgba_t x) { return (x >>  8 & 0xff) / 255.f; }
ic f32 rgba_a(rgba_t x) { return (x       & 0xff) / 255.f; }

ic rgba_t rgba(f32 r, f32 g, f32 b, f32 a)
{
  return u32(r * 255) << 24
       | u32(g * 255) << 16
       | u32(b * 255) << 8
       | u32(a * 255);
}


u64 xcb_keycode_codepoint(Stc &keymap, uf8 keycode, uf16 mods);
St  xcb_keycode_meta     (Stc &keymap, uf8 keycode, uf16 mods);


}

#include "../end.hh"

#endif
