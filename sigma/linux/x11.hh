#ifndef σlinux_x11_h
#define σlinux_x11_h

#include "../../sigma.hh"
#include "../begin.hh"

namespace σ
{

using namespace τ;


u64 xcb_keycode_codepoint(Stc &keymap, uf8 keycode, uf16 mods);
St  xcb_keycode_meta     (Stc &keymap, uf8 keycode, uf16 mods);


}

#include "../end.hh"

#endif
