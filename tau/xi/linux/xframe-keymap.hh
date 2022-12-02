#ifndef τξlinux_xframe_keymap_h
#define τξlinux_xframe_keymap_h


#include <xcb/xcb.h>
#include <xcb/xproto.h>

#include "../../utf9-types.hh"


#include "../begin.hh"

namespace τ::ξ
{


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


u9_symbol keymap_us_meta[128] =
{
  // NOTE: this table's first entry is scancode 8, not 0
  "none"_y,                                         // 8 is undefined
  "esc"_y,                                          // 9 is escape
  "none"_y, "none"_y, "none"_y, "none"_y,
  "none"_y, "none"_y, "none"_y, "none"_y,
  "none"_y, "none"_y,                               // 10-19 are digits

  "none"_y, "none"_y, "backspace"_y,                // 20-22
  "tab"_y,                                          // 23
  "none"_y, "none"_y, "none"_y, "none"_y,
  "none"_y, "none"_y,
  "none"_y, "none"_y, "none"_y, "none"_y,
  "none"_y,                                         // 24-35

  "enter"_y,                                        // 36
  "lctrl"_y,                                        // 37 = left ctrl

  "none"_y, "none"_y, "none"_y, "none"_y,
  "none"_y, "none"_y, "none"_y, "none"_y,
  "none"_y, "none"_y, "none"_y, "none"_y,           // 38-49
  "lshift"_y,                                       // 50 = left shift

  "none"_y, "none"_y, "none"_y, "none"_y,
  "none"_y, "none"_y, "none"_y, "none"_y,
  "none"_y, "none"_y, "none"_y,                     // 51-61
  "rshift"_y,                                       // 62 = right shift

  "none"_y,
  "lalt"_y,
  "none"_y,                                         // 65 = space
  "capslock"_y,
  "f1"_y, "f2"_y, "f3"_y, "f4"_y, "f5"_y,
  "f6"_y, "f7"_y, "f8"_y, "f9"_y, "f10"_y,          // 76 = F19

  "numlock"_y,                                      // 77 = num lock
  "scrolllock"_y,                                   // 78 = scroll lock

  "none"_y, "none"_y, "none"_y, "none"_y,
  "none"_y, "none"_y, "none"_y, "none"_y,
  "none"_y, "none"_y, "none"_y, "none"_y,
  "none"_y,                                         // 79-91 = numpad

  "none"_y, "none"_y, "none"_y,                     // ???
  "f11"_y, "f12"_y,                                 // 96 = F12

  // TODO: fill out the rest of these using xev to reverse-engineer
  // (although I guess they will be thinkpad-specific)
  "none"_y, "none"_y, "none"_y, "none"_y,
  "none"_y, "none"_y, "none"_y, "none"_y,
  "none"_y, "none"_y, "none"_y, "none"_y,
  "none"_y, "none"_y, "none"_y, "none"_y,
  "none"_y, "none"_y, "none"_y, "none"_y,
  "none"_y, "none"_y, "none"_y, "none"_y,
  "none"_y, "none"_y, "none"_y, "none"_y,
  "none"_y, "none"_y, "none"_y, "none"_y,
  "none"_y, "none"_y, "none"_y, "none"_y,
  "none"_y, "none"_y, "none"_y, "none"_y,
};


ch xcb_keycode_ascii(uf8 xcb_keycode, uf16 mods = 0)
{
  return mods & XCB_MOD_MASK_SHIFT
       ? keymap_us_shift[xcb_keycode - 8]
       : keymap_us      [xcb_keycode - 8];
}


u9_symbol xcb_keycode_meta(uf8 xcb_keycode)
{
  return keymap_us_meta[xcb_keycode - 8];
}


}

#include "../end.hh"



#endif
