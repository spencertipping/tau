#ifndef τξlinux_xframe_xcbio_h
#define τξlinux_xframe_xcbio_h


#include <errno.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

#include "xframe-keymap.hh"

#include "../begin.hh"


namespace τ
{


// TODO: event → window demultiplexer
// (probably a component for an XCB connection)


// TODO: factor this logic out to something more generic; we shouldn't
// have to write structs this way
struct o9xcbe
{
  sletc             sb = u9sb(u9sq(sizeof(xcb_generic_event_t) + 1));
  iN               &n;
  iN               &e;
  xcb_connection_t *c;
  uNc               fd;

  o9xcbe(iN &n_, iN &e_, xcb_connection_t *c_)
    : n{n_}, e{e_}, c{c_}, fd{Sc<uN>(xcb_get_file_descriptor(c))} {}

  uN size() const { return 1 + sizeof(xcb_generic_event_t) + sb; }
  uN write(ζp m) const
    { xcb_generic_event_t *x;
      n = !!(x = xcb_poll_for_event(c));
      if (!n) { n = -1; e = EAGAIN; return ζω; }
      e = 0;
      let i = u9ws(m, 0, u9t::build, 1 + sizeof(xcb_generic_event_t));
      W(m + i, 0, Sc<u8>(u9_build::istruct));
      memcpy(m + i + 1, x, sizeof(xcb_generic_event_t));
      free(x);
      return 0; }
};

template<> struct o9_<o9xcbe> { sletc v = true; };


template<>
inline void Φf<o9xcbe>::init()
{
  ep = f << *this ? 0 : errno;
}

template<>
inline Φf<o9xcbe>::~Φf()
{
  f.x(*this);
}


namespace ξ
{


ϝ &xcb_events(Φ &f, xcb_connection_t *c)
{
  return *new ϝ(f, "xcb_events", ϝ::ξΦ, [&, c](ϝ &f, γ &g)
    { Φf<o9xcbe> i{g.f, c}; while (f << i); });
}


ϝ &xcb_event_decode(Φ &f)
{
  return *new ϝ(f, "xcb_event_decode", ϝ::ξι, [](ϝ &f)
    { for (let x : f)
        if (x.is_istruct())
        { let a = Sc<xcb_generic_event_t*>(x);
          let t = a->response_type & ~0x80;
          switch (t)
          {
          case XCB_EXPOSE:
          { let e = Rc<xcb_expose_event_t*>(a);
            f << o9t("type"_y, "expose"_y,
                     "win"_y,   e->window,
                     "x"_y,     e->x,
                     "y"_y,     e->y,
                     "w"_y,     e->width,
                     "h"_y,     e->height,
                     "count"_y, e->count).m();
            break; }

          case XCB_BUTTON_PRESS:
          case XCB_BUTTON_RELEASE:
          case XCB_MOTION_NOTIFY:
          { let e = Rc<xcb_button_press_event_t*>(a);
            f << o9t("type"_y, t == XCB_BUTTON_PRESS ? "button_press"_y : t == XCB_BUTTON_RELEASE ? "button_release"_y : "motion"_y,
                     "t"_y,           e->time,
                     "root"_y,        e->root,
                     "event"_y,       e->event,
                     "child"_y,       e->child,
                     "root_x"_y,      e->root_x,
                     "root_y"_y,      e->root_y,
                     "event_x"_y,     e->event_x,
                     "event_y"_y,     e->event_y,
                     "state"_y,       e->state,
                     "same_screen"_y, e->same_screen).m();
            break; }

          case XCB_ENTER_NOTIFY:
          case XCB_LEAVE_NOTIFY:
          { let e = Rc<xcb_enter_notify_event_t*>(a);
            f << o9t("type"_y, t == XCB_ENTER_NOTIFY ? "enter"_y : "leave"_y,
                     "t"_y,                 e->time,
                     "root"_y,              e->root,
                     "event"_y,             e->event,
                     "child"_y,             e->child,
                     "root_x"_y,            e->root_x,
                     "root_y"_y,            e->root_y,
                     "event_x"_y,           e->event_x,
                     "event_y"_y,           e->event_y,
                     "state"_y,             e->state,
                     "mode"_y,              e->mode,
                     "same_screen_focus"_y, e->same_screen_focus).m();
            break; }

          case XCB_KEY_PRESS:
          case XCB_KEY_RELEASE:
          { let e = Rc<xcb_key_press_event_t*>(a);
            char ks[2] {0, 0};
            ks[0] = xcb_keycode_ascii(e->detail, e->state);
            f << o9t("type"_y, t == XCB_KEY_PRESS ? "key_press"_y : "key_release"_y,
                     "t"_y,           e->time,
                     "keycode"_y,     e->detail,
                     "keyascii"_y,    Sc<chc*>(ks),
                     "keymeta"_y,     xcb_keycode_meta(e->detail),
                     "root"_y,        e->root,
                     "event"_y,       e->event,
                     "child"_y,       e->child,
                     "root_x"_y,      e->root_x,
                     "root_y"_y,      e->root_y,
                     "event_x"_y,     e->event_x,
                     "event_y"_y,     e->event_y,
                     "state"_y,       e->state,
                     "same_screen"_y, e->same_screen).m();
            break; }
          }
        } });
}


}
}


#include "../end.hh"


#endif
