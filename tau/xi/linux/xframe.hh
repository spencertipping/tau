#ifndef τξlinux_xframe_h
#define τξlinux_xframe_h


#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>


#include "xframe-keymap.hh"
#include "xframe-gltext.hh"

#include "../begin.hh"

namespace τ::ξ
{


struct xframe
{

  sletc fa = 0x1;        // flag: frame is active
  sletc fr = 0x2;        // flag: frame needs to be redrawn

  uN   w  = 0;
  uN   h  = 0;
  uf8  fs = 0;
  uf8  b  = 20;
  u8  *rb = nullptr;  // render buffer (of i9 render commands)
  u8  *ob = nullptr;  // offscreen buffer (of inbound i9s)


};


}

#include "../end.hh"


#endif
