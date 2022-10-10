//#define τdebug_i9st 1

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


int u9dasm_seq(int argc, char **argv)
{
  Φ f;
  iota(f, atoi(argv[0])) | utf9_dasm(f, 32768) | fd_out(f, 1);
  f.go();
  return 0;
}


int u9dasm(int argc, char **argv)
{
  Φ f;
  fd_in(f, 0)
    | utf9_asm(f)
    | utf9_dasm(f, argc ? atoi(argv[0]) : 0)
    | utf9_dasm(f)
    | stream_out(f, std::cout);
  f.go();
  return 0;
}


int u9asm(int argc, char **argv)
{
  Φ f;
  fd_in(f, 0) | utf9_asm(f) | stream_out(f, std::cout);
  f.go();
  return 0;
}


int chat(int argc, char **argv)
{
  Φ f;
  auto &b = broadcast(f);
  tcp_server(f, argc ? atoi(argv[0]) : 3000)
    | io<i9, fd_t>(f, [&](i9 x)
      { let fd = Sc<u9_scoped<u9_Φ, fd_t>*>(x[0])->x;
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


int acat(int argc, char **argv)
{
  Φ f;
  fd_in(f, 0) | utf9_dasm(f) | utf9_asm(f) | fd_out(f, 1);
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
    for (f32 dy = 1.f; dy <= 100; dy += 5)
      p << o9t((uN) xrop_line,
               x,                                                           y - dy, 1.f,
               x + 500.f + 100.f * sinf(f.dt() / 1ms / 1000.f + dy / 40.f), y - dy, 1.f,
               1.f, 0xe0e0f080);
    for (f32 z = 1; z < 10; z += 0.1)
      p << o9t((uN) xrop_text, "Gentium 48", "τ« //ξ", 0xe0e0f0ff, x, y, z);
    p << u9τ; }) | tee(f);

  e ^ s;
  //e | xframe(f);  // bogus on the X11 side, but tau works fine

  s & sink<i9>(f, [cx, cy, cw, ch](i9 a) {
    if (!a.is_istruct()) return;
    let e = Sc<xcb_generic_event_t*>(a);
    if ((e->response_type & ~0x80) == XCB_MOTION_NOTIFY)
    { let me = Rc<xcb_motion_notify_event_t*>(e);
      *cx = me->event_x;
      *cy = me->event_y;
      cout << "mouse move " << *cx << ", " << *cy << endl; }
    else if ((e->response_type & ~0x80) == XCB_CONFIGURE_NOTIFY)
    { let ce = Rc<xcb_configure_notify_event_t*>(e);
      *cw = ce->width;
      *ch = ce->height;
      cout << "set w,h " << *cw << ", " << *ch << endl; }});

  f.go();
  return 0;
}


int editor(int argc, char **argv)
{
  Φ f;
  f32 cx = 50;
  f32 cy = 50;
  f32 cw = 100;
  f32 ch = 100;

  f32 lx = 10;
  f32 ly = 10;
  f32 lh = 1.2;

  int ex = 0;
  int ey = 0;

  V<St> d{
    "#include <iostream>",
    "",
    "int main(int argc, char **argv)",
    "{",
    "  std::cout << argv[0] << std::endl;",
    "  return 0;",
    "}"
  };

  V<St> kw{
    "int", "long", "short", "char", "signed", "unsigned", "float", "double",
    "return", "if", "switch", "case", "default", "volatile",
    "struct", "typedef", "//", "/*", "*/"
  };

  θr(f, 250ms)
    | each<i9>(f, [&](i9, γφ &p) { p << o9t("Ubuntu Mono 36", "x"); })
    | text_size(f)
    | sink<i9>(f, [&](i9 x) { lx = x[2]; ly = x[3]; });

  auto &s = xframe(f) | splitter(f);
  //auto &es = s & tee(f);

  auto &e = θr(f, 30ms) |
    //es |
    each<i9>(f, [&](i9, γφ &p) {
      for (let &l : d) p << o9t("Ubuntu Mono 36", l);
      p << u9τ;
    }) |
    each<i9>(f, [&](i9 l, γφ &p) {
      let t  = f.dt() / 1ms * 0.001f;

      let ox = -cw / 2 + 80.f + 100.f * sinf(t);
      let oy = -ch / 2        + 100.f * cosf(t);
      let oz = 1.1f + 0.1f * sinf(t / 2);
      let tx = 8.f;

      static uN  n = 0;
      if (!l)
      { n = 0;
        p << o9t((uN) xrop_line,
                 ox + tx + ex * lx, oy + (ey % 16)     * ly * lh, oz + ((ey >> 4) + 1)*.3f,
                 ox + tx + ex * lx, oy + (ey % 16 + 1) * ly * lh, oz + ((ey >> 4) + 1)*.3f,
                 2.f, 0xf0a040ffu);

        for (let &k : kw)
          for (int i = 0; i < d.size(); ++i)
            for (uN s = 0; (s = d[i].find(k, s)) != string::npos; ++s)
            { let w = k.size();
              p << o9t((uN) xrop_line,
                       ox + tx + lx * s,       oy + (i % 16 + 1) * ly * lh, oz + ((i >> 4) + 1) * .3f,
                       ox + tx + lx * (s + w), oy + (i % 16 + 1) * ly * lh, oz + ((i >> 4) + 1) * .3f,
                       1.f, 0xe0e0e0ffu); }

        p << l;
        return; }
      let y = oy + (n % 16) * ly * lh;
      ++n;
      p << o9t((uN) xrop_line,
               ox, y,      oz + ((n >> 4) + 1)*0.3f,
               ox, y + ly, oz + ((n >> 4) + 1)*0.3f, 1.f, 0x808080f0u);
      p << o9t((uN) xrop_text, l[0], l[1], 0xf0f0f0f0u, ox + tx, y, oz + ((n >> 4) + 1) * 0.3f);
      p << o9t((uN) xrop_text, "Gentium Bold 30", to_string(n), n == ey + 1 ? 0xf0a040ff : 0x606060ff, ox - 50.f, y + 8.f, oz + ((n >> 4) + 1) * 0.3f);
    }) | tee(f);

  e ^ s;
  //e | xframe(f);  // bogus on the X11 side, but tau works fine

  s & sink<i9>(f, [&](i9 a) {
    if (!a.is_istruct()) return;
    let e = Sc<xcb_generic_event_t*>(a);
    let t = e->response_type & ~0x80;
    if (t == XCB_MOTION_NOTIFY)
    { let me = Rc<xcb_motion_notify_event_t*>(e);
      cx = me->event_x;
      cy = me->event_y; }
    else if (t == XCB_CONFIGURE_NOTIFY)
    { let ce = Rc<xcb_configure_notify_event_t*>(e);
      cw = ce->width;
      ch = ce->height; }
    else if (t == XCB_KEY_PRESS)
    { let ke = Rc<xcb_key_press_event_t*>(e);
      if (let a = xcb_keycode_ascii(ke->detail, ke->state))
        if (a == '\b')
        {
          if (ex)
          { auto l   = d[ey].substr(0, ex);
            auto nex = ex;
            while (l.size() && (l[-1] & 0xc0) == 0x80) --nex, l.pop_back();
            if (l.size()) --nex, l.pop_back();
            l.append(d[ey].substr(ex));
            ex = nex;
            d[ey] = l; }
          else if (ey)
          { ex = d[ey - 1].size();
            d[ey - 1].append(d[ey]);
            d.erase(d.begin() + ey);
            ey--; }
        }
        else if (a == '\n')
        { auto l = d[ey].substr(0, ex);
          auto r = d[ey].substr(ex);
          ++ey;
          d.insert(d.begin() + ey, r);
          d[ey - 1] = l;
          ex = 0; }
        else
        { auto l = d[ey].substr(0, ex);
          l.push_back(a);
          l.append(d[ey].substr(ex));
          ++ex;
          d[ey] = l; }
      else
        switch (ke->detail)
        {
        case 111: --ey; if (ey < 0) ey = 0;                       break;  // up arrow
        case 116: ++ey; if (ey >= d.size()) ey = d.size() - 1;    break;  // down arrow
        case 113: --ex; if (ex < 0) ex = 0;                       break;  // left arrow
        case 114: ++ex; if (ex > d[ey].size()) ex = d[ey].size(); break;  // right arrow
        case 110: ex = 0; break;             // home
        case 115: ex = d[ey].size(); break;  // end
        default:
          cout << "key press: " << Sc<uN>(ke->detail) << endl;
          break;
        }

      if (ex > d[ey].size()) ex = d[ey].size();
    }});

  f.go();
  return 0;
}


int main(int argc, char **argv)
{
  if (!strcmp(argv[1], "nseq"))   return nop_seq   (argc - 2, argv + 2);
  if (!strcmp(argv[1], "tseq"))   return tau_seq   (argc - 2, argv + 2);
  if (!strcmp(argv[1], "fseq"))   return tau_fseq  (argc - 2, argv + 2);
  if (!strcmp(argv[1], "useq"))   return u9dasm_seq(argc - 2, argv + 2);
  if (!strcmp(argv[1], "udasm"))  return u9dasm    (argc - 2, argv + 2);
  if (!strcmp(argv[1], "uasm"))   return u9asm     (argc - 2, argv + 2);
  if (!strcmp(argv[1], "chat"))   return chat      (argc - 2, argv + 2);
  if (!strcmp(argv[1], "cat"))    return cat       (argc - 2, argv + 2);
  if (!strcmp(argv[1], "acat"))   return acat      (argc - 2, argv + 2);
  if (!strcmp(argv[1], "x11"))    return x11       (argc - 2, argv + 2);
  if (!strcmp(argv[1], "editor")) return editor    (argc - 2, argv + 2);

  cerr << "unknown command " << argv[1] << endl;
  return 1;
}


#include "../tau/end.hh"


#else

int main()
{
  std::cerr << "γ not defined" << std::endl;
  return 1;
}

#endif
