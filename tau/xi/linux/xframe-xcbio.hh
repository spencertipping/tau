#ifndef τξlinux_xframe_xcbio_h
#define τξlinux_xframe_xcbio_h


#include <errno.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>


#include "../begin.hh"


namespace τ
{


// TODO: event → window demultiplexer
// (probably a component for an XCB connection)


struct o9xcbe
{
  sletc             sb = u9sb(u9sq(sizeof(xcb_generic_event_t)));
  iN               &n;
  iN               &e;
  xcb_connection_t *c;
  uNc               fd;

  o9xcbe(iN &n_, iN &e_, xcb_connection_t *c_)
    : n{n_}, e{e_}, c{c_}, fd{Sc<uN>(xcb_get_file_descriptor(c))} {}

  uN size() const { return sizeof(xcb_generic_event_t) + sb; }
  uN write(ζp m) const
    { xcb_generic_event_t *x;
      n = !!(x = xcb_poll_for_event(c));
      if (!n) { n = -1; e = EAGAIN; return ζω; }
      e = 0;
      let i = u9ws(m, 0, u9t::nstruct, sizeof(xcb_generic_event_t));
      memcpy(m + i, x, sizeof(xcb_generic_event_t));
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
  return *new ϝ(f, ϝ::ξΦ, [&, c](ϝ &f, γ &g)
    { Φf<o9xcbe> i{g.f, c}; while (f << i); });
}


}
}


#include "../end.hh"


#endif
