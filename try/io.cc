//#define tau_debug_i9st 1

#include "../tau.hh"

#if τdefines_γ


#include <cstring>
#include <xcb/xproto.h>


#include "../tau/begin.hh"

using namespace std;
using namespace τ;
using namespace τ::ξ;


int nop_seq(int argc, char **argv)
{
  Φ f;
  for (let x : ι(atoi(argv[0]))) cout << f.dt() << ": " << x << endl;
  return 0;
}


int tau_seq(int argc, char **argv)
{
  Φ f;
  iota(f, atoi(argv[0])) | Δθ(f, 50ms) | stream_out(f, std::cout);
  f.go();
  return 0;
}


int tau_fseq(int argc, char **argv)
{
  Φ f;
  iota(f, atoi(argv[0])) | stream_out(f, std::cout);
  f.go();
  return 0;
}


int chat(int argc, char **argv)
{
  Φ f;
  auto &b = broadcast(f);
  tcp_server(f, argc ? atoi(argv[0]) : 3000)
    | io<i9, uN>(f, [&](i9 x)
      { let fd = Sc<u9_pidfd>(x[0]).fd;
        b | fd_io(f, fd);
        return fd; })
    | stream_out(f, cout);

  b & stream_out(f, cout);

  f.go();
  return 0;
}


int cat(int argc, char **argv)
{
  Φ f;
  fd_in(f, 0) | fd_out(f, 1);
  f.go();
  return 0;
}


int x11(int argc, char **argv)
{
  Φ f;
  let cx = new f32{50};
  let cy = new f32{50};
  let cw = new f32{100};
  let ch = new f32{100};

  auto &s = xframe(f) | splitter(f);
  auto &e = θr(f, 30ms) | each<i9>(f, [&, cx, cy, cw, ch](i9, γφ &p) {
    let x  = *cx + 40 - *cw / 2;
    let y  = *cy      - *ch / 2;
    for (f32 dy = 1.f + 1e-8; dy <= 100; dy += 5)
      p << o9t((uN) xrop_line,
               x,                                                           y - dy, 1.f,
               x + 500.f + 100.f * sinf(f.dt() / 1ms / 1000.f + dy / 40.f), y - dy, 1.f,
               1.f, 0xe0e0f080);
    for (f32 z = 1; z < 10; ++z)
      p << o9t((uN) xrop_text, "Gentium 48", "τ« //ξ", 0xe0e0f0ff, x, y, z);
    p << u9τ; }) | tee(f);

  e ^ s;
  //e | xframe(f);  // bogus on the X11 side, but tau works fine

  s & sink<i9>(f, [cx, cy, cw, ch](i9 a) {
    if (a.type() != u9t::nstruct) return;
    let e = Rc<xcb_generic_event_t*>(a.data());
    if ((e->response_type & ~0x80) == XCB_MOTION_NOTIFY)
    { let me = Rc<xcb_motion_notify_event_t*>(e);
      *cx = me->event_x;
      *cy = me->event_y; }
    else if ((e->response_type & ~0x80) == XCB_CONFIGURE_NOTIFY)
    { let ce = Rc<xcb_configure_notify_event_t*>(e);
      *cw = ce->width;
      *ch = ce->height;
      cout << "set w,h " << *cw << ", " << *ch << endl;
    }});

  f.go();
  return 0;
}


int main(int argc, char **argv)
{
  if (!strcmp(argv[1], "nseq")) return nop_seq (argc - 2, argv + 2);
  if (!strcmp(argv[1], "tseq")) return tau_seq (argc - 2, argv + 2);
  if (!strcmp(argv[1], "fseq")) return tau_fseq(argc - 2, argv + 2);
  if (!strcmp(argv[1], "chat")) return chat    (argc - 2, argv + 2);
  if (!strcmp(argv[1], "cat"))  return cat     (argc - 2, argv + 2);
  if (!strcmp(argv[1], "x11"))  return x11     (argc - 2, argv + 2);
  return 0;
}


#include "../tau/end.hh"


#else

int main()
{
  std::cerr << "γ not defined" << std::endl;
  return 1;
}

#endif
