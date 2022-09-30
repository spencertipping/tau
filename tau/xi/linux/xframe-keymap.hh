#ifndef τξlinux_xframe_keymap_h
#define τξlinux_xframe_keymap_h


#include <xcb/xcb.h>
#include <xcb/xproto.h>


#include "../begin.hh"

namespace τ::ξ
{


// Meta-keys: things that aren't visible characters
enum class xkm : u8
{
  none,
  esc,
  home,
  end,
  left,
  right,
  up,
  down,
  tab,
  enter,
  backspace,
  del,
  insert,
  lalt,
  ralt,
  lctrl,
  rctrl,
  lsuper,
  rsuper,
  lshift,
  rshift,
  f1, f2,  f3,  f4,
  f5, f6,  f7,  f8,
  f9, f10, f11, f12,
  numlock,
  capslock,
  scrolllock,
};

typedef xkm const xkmc;


// NOTE: https://stackoverflow.com/questions/18689863/obtain-keyboard-layout-and-keysyms-with-xcb
// less good: https://stackoverflow.com/questions/34582279/linux-c-keymapping-keycodes


// https://stackoverflow.com/questions/61124564/convert-scancodes-to-ascii
chc keymap_us[128] =
{
  // NOTE: this table's first entry is scancode 8, not 0
  0,                                                 // 8 is undefined
  27,                                                // 9 is escape
  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',  // 10-19 are digits
  '-', '=', '\b',                                    // 20-22
  '\t',                                              // 23
  'q', 'w', 'e', 'r', 't', 'y', 'u',
  'i', 'o', 'p', '[', ']',                           // 24-35
  '\n',                                              // 36
  0,                                                 // 37 = left ctrl
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
  '\'', '`',                                         // 38-49
  0,                                                 // 50 = left shift
  '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm',
  ',', '.', '/',                                     // 51-61
  0,                                                 // 62 = right shift

  '*',                              // 63 = num *
  0,                                // 64 = left alt
  ' ',                              // 65 = space
  0,                                // 66 = caps lock
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     // 67-76 = F1-F10
  0,                                // 77 = num lock
  0,                                // 78 = scroll lock
  0, 0, 0, '-', 0, 0, 0, '+',
  0, 0, 0, 0, 0,                    // 79-91 = numpad
  0, 0, 0,                          // 92 = alt-sysrq, 93=?, 94=?
  0, 0,                             // 95, 96 = F11, F12
  0,                                // all else undefined
};

chc keymap_us_shift[128] =
{
  // NOTE: this table's first entry is scancode 8, not 0
  0,                                                 // 8 is undefined
  27,                                                // 9 is escape
  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',  // 10-19 are digits
  '_', '+', '\b',                                    // 20-22
  '\t',                                              // 23
  'Q', 'W', 'E', 'R', 'T', 'Y', 'U',
  'I', 'O', 'P', '{', '}',                           // 24-35
  '\n',                                              // 36
  0,                                                 // 37 = left ctrl
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
  '"', '~',                                          // 38-49
  0,                                                 // 50 = left shift
  '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M',
  '<', '>', '?',                                     // 51-61
  0,                                                 // 62 = right shift

  '*',                              // 63 = num *
  0,                                // 64 = left alt
  ' ',                              // 65 = space
  0,                                // 66 = caps lock
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     // 67-76 = F1-F10
  0,                                // 77 = num lock
  0,                                // 78 = scroll lock
  0, 0, 0, '-', 0, 0, 0, '+',
  0, 0, 0, 0, 0,                    // 79-91 = numpad
  0, 0, 0,                          // 92 = alt-sysrq, 93=?, 94=?
  0, 0,                             // 95, 96 = F11, F12
  0,                                // all else undefined
};


xkmc keymap_us_meta[128] =
{
  // NOTE: this table's first entry is scancode 8, not 0
  xkm::none,                                         // 8 is undefined
  xkm::esc,                                          // 9 is escape
  xkm::none, xkm::none, xkm::none, xkm::none,
  xkm::none, xkm::none, xkm::none, xkm::none,
  xkm::none, xkm::none,                              // 10-19 are digits

  xkm::none, xkm::none, xkm::backspace,              // 20-22
  xkm::tab,                                          // 23
  xkm::none, xkm::none, xkm::none, xkm::none,
  xkm::none, xkm::none,
  xkm::none, xkm::none, xkm::none, xkm::none,
  xkm::none,                                         // 24-35

  xkm::enter,                                        // 36
  xkm::lctrl,                                        // 37 = left ctrl

  xkm::none, xkm::none, xkm::none, xkm::none,
  xkm::none, xkm::none, xkm::none, xkm::none,
  xkm::none, xkm::none, xkm::none, xkm::none,        // 38-49
  xkm::lshift,                                       // 50 = left shift

  xkm::none, xkm::none, xkm::none, xkm::none,
  xkm::none, xkm::none, xkm::none, xkm::none,
  xkm::none, xkm::none, xkm::none,                   // 51-61
  xkm::rshift,                                       // 62 = right shift

  xkm::none,
  xkm::lalt,
  xkm::none,                                         // 65 = space
  xkm::capslock,
  xkm::f1, xkm::f2, xkm::f3, xkm::f4, xkm::f5,
  xkm::f6, xkm::f7, xkm::f8, xkm::f9, xkm::f10,      // 76 = F19

  xkm::numlock,                                      // 77 = num lock
  xkm::scrolllock,                                   // 78 = scroll lock

  xkm::none, xkm::none, xkm::none, xkm::none,
  xkm::none, xkm::none, xkm::none, xkm::none,
  xkm::none, xkm::none, xkm::none, xkm::none,
  xkm::none,                                         // 79-91 = numpad

  xkm::none, xkm::none, xkm::none,                   // ???
  xkm::f11, xkm::f12,                                // 96 = F12

  // TODO: fill out the rest of these using xev to reverse-engineer
  // (although I guess they will be thinkpad-specific)
  xkm::none,                                         // all else undefined
};


ch xcb_keycode_ascii(uf8 xcb_keycode, uf16 mods = 0)
{
  return mods & XCB_MOD_MASK_SHIFT
    ? keymap_us_shift[xcb_keycode - 8]
    : keymap_us      [xcb_keycode - 8];
}


xkm xcb_keycode_meta(uf8 xcb_keycode)
{
  return keymap_us_meta[xcb_keycode - 8];
}


}

#include "../end.hh"



#endif
