#ifndef τπabi1_h
#define τπabi1_h


#include "arch.hh"

#include "../dep/pffft.h"

#include "pi-gc-heapview.hh"
#include "utf9-i9.hh"
#include "utf9-types.hh"
#include "utf9.hh"

#include "pi-types.hh"
#include "pi-gc.hh"
#include "pi-abi.hh"
#include "pi-int.hh"

#include "Phi.hh"


#include "pi-abi-begin.hh"

namespace τ
{


void πabi1_const(πabi &a)
{
  a .def("t", I{ i.dpush(true);  })
    .def("f", I{ i.dpush(false); });
}


void πabi1_blocks(πabi &a)
{
  a .def("[",  I{ i.dpush(Sc<πbi>(i.rs.back())); i.rs.back() += n; })
    .def("]",  I{ i.rs.pop_back(); })
    .def("[|", I{ i.fpush(n); })
    .def("|]", I{ i.fpop(); });
}


void πabi1_stack(πabi &a)
{
  a .def(":",  I{ i << i[0]; })
    .def("::", I{ i << i[1]; i << i[1]; })
    .def("^",  I{ i << i[n ? n : 1]; })
    .def("^^", I{ i << i[2]; })
    .def("_",  I{ i.drop(n ? n : 1); })
    .def("%",  I{
        let a = i[0];
        let b = i[1];
        i.drop(2);
        i << a;
        i << b; });
}


void πabi1_frame(πabi &a)
{
  a .def("&:",  I{ i.fpush(Sc<uN>(i.dpop())); })
    .def("&:'", I{ i.fpush(n); })
    .def("&_",  I{ i.fpop(); })
    .def("&@'", I{ i << i.fi(n); })
    .def("&='", I{ i.fi(n) = i.pop(); })
    .def("&@",  I{ i << i.fi(Sc<uN>(i.dpop())); })
    .def("&=",  I{
        let v = Sc<uN>(i.dpop());
        i.fi(v) = i.pop(); });
}


void πabi1_global(πabi &a)
{
  a .def("%@", I{ let k = i.dpop(); let v = i.gs()[k]; A(i9{v}.deref().exists(), "undefined global " << k); i << v; })
    .def("%=", I{ let s = i.dpop(); i.gs()[s] = i.dpop(); })
    .def("%_", I{ i.gs().x(i.dpop()); })
    .def("%?", I{ i.dpush(i.gs().i(i.dpop())); });
}


void πabi1_control(πabi &a)
{
  a .def(".",  I{ i.rs.push_back(i.bpop()); })
    .def("?",  I{
        let e = i[0];
        let t = i[1];
        let c = i[2];
        i.drop(3);
        i << (Sc<bool>(c) ? t : e); })
    .def("?.", I{
        let e = i.bpop();
        let t = i.bpop();
        let c = i.dpop().b();
        i.run(c ? t : e); })
    .def("?!", I{
        let c = i.bpop();
        let b = i.bpop();
        πrsf r{i};
      loop:
        i.run(c);
        if (r && i.dpop().b()) { i.run(b); goto loop; }})
    .def("!!", I{ let b = i.bpop(); πrsf r{i}; while (r) i.run(b); })
    .def(".^", I{ i.rs.resize(i.rs.size() - n - 1); });
}


void πabi1_quote(πabi &a)
{
  a .def("i8",   I{ i.dpush(Sc<i8>(n)); })
    .def("i16",  I{ i.dpush(Sc<i16>(n)); })
    .def("i32",  I{ i.dpush(Sc<i32>(n)); })
    .def("i64",  I{ i.dpush(Sc<i64>(n)); })
    .def("iN",   I{ i.dpush(Sc<iN>(n)); })
    .def("utf8", I{ i.dpush(i9{Cc<ζp>(i.p->q.data() + n)}); })
    .def("sym",  I{ i.dpush(i9{Cc<ζp>(i.p->q.data() + n)}); });
}


void πabi1_u9_general(πabi &a)
{
  a .def("?e", I{ i.dpush(i.dpop().exists()); })
    .def("?t", I{ i.dpush(u9typesyms.at(i.dpop().type())); })
    .def("?f", I{ i.dpush(i.dpop().flagged()); })
    .def("?s", I{ i.dpush(i.dpop().size()); })
    .def("?S", I{ i.dpush(i.dpop().osize()); })
    .def(":f", I{if (i[0].flagged()) i << (i.h << πo9f{i.pop()}); })
    .def(":h", I{ i.dpush(i.dpop().h()); })
    .def("==", I{ i.dpush(i.dpop() == i.dpop()); })
    .def("≡", I{ i.dpush(i.dpop().deref().a == i.dpop().deref().a); });
}


void πabi1_u9_vector(πabi &a)
{
#define a1vfns(t)                                                      \
  a .def("»" #t "s", I{                                                \
      uNc k = Sc<iN>(i.dpop());                                        \
      let r = i9{i.h << o9vec<t>{k}};                                  \
      for (uN j = 0; j < k; ++j)                                       \
        r.template set<t>(j, i.dpop().template at<t>(0));              \
      i << r; })                                                       \
    .def(#t "s#", I{ i.dpush(Sc<u64>(i.dpop().vn())); })               \
    .def(#t "s@", I{                                                   \
        uNc k = Sc<iN>(i.dpop());                                      \
        i.dpush(i.dpop().template at<t>(k)); })                        \
    .def(#t "s!", I{                                                   \
        πhnf f{i.h, 2};                                                \
        uNc sn = Sc<iN>(i.dpop());                                     \
        uNc si = Sc<iN>(i.dpop());                                     \
        i9 s = i.dpop();            f(&s);                             \
        i9 r = i.h << o9vec<t>{sn}; f(&r);                             \
        for (uN j = si; j < sn; ++j)                                   \
          r.template set<t>(j - si, s.template at<t>(j));              \
        i << r; })                                                     \
    .def(#t "s++", I{                                                  \
        πhnf f{i.h, 3};                                                \
        i9 x = i.dpop(); f(&x);                                        \
        i9 y = i.dpop(); f(&y);                                        \
        i9 z  = i.h << o9vec<t>{x.vn() + y.vn()}; f(&z);               \
        for (uN j = 0; j < x.vn(); ++j)                                \
          z.template set<t>(j, x.template at<t>(j));                   \
        for (uN j = 0; j < y.vn(); ++j)                                \
          z.template set<t>(j + x.vn(), y.template at<t>(j));          \
        i << z; })                                                     \
    .def(#t "s.", I{                                                   \
        πhnf f{i.h, 1};                                                \
        let g = i.bpop();                                              \
        πrsf r{i};                                                     \
        i9 xs = i.dpop(); f(&xs);                                      \
        for (uN j = 0; r && j < xs.vn(); ++j)                          \
        { i.dpush(xs.template at<t>(j));                               \
          i.run(g); } })

  typedef bool b; a1vfns(b);

  a1vfns(i8);  a1vfns(i16); a1vfns(i32); a1vfns(i64);  a1vfns(iN);
  a1vfns(u8);  a1vfns(u16); a1vfns(u32); a1vfns(u64);  a1vfns(uN);
  a1vfns(f32); a1vfns(f64);
  a1vfns(c32); a1vfns(c64);

#undef a1vfns
}


void πabi1_u9_number(πabi &a)
{
#define a1sc(t) a.def(">"#t,    I{ i.dpush(i.dpop().template at<t>(0)); });
#define a1vc(t) a.def(">"#t"s", I{                              \
      πhnf f{i.h, 2};                                           \
      i9 x = i.pop(); f(&x);                                    \
      i9 y = i.h << o9vec<t>{x.vn()}; f(&y);                    \
      i << y;                                                   \
      let k = x.vn();                                           \
      for (uN i = 0; i < k; ++i)                                \
        y.template set<t>(i, x.template at<t>(i)); });

  typedef bool b;
  a1sc(b);   a1vc(b);

  a1sc(i8);  a1vc(i8);   a1sc(u8);  a1vc(u8);
  a1sc(i16); a1vc(i16);  a1sc(u16); a1vc(u16);
  a1sc(i32); a1vc(i32);  a1sc(u32); a1vc(u32);  a1sc(iN); a1sc(uN);
  a1sc(i64); a1vc(i64);  a1sc(u64); a1vc(u64);  a1vc(iN); a1vc(uN);

  a1sc(f32); a1vc(f32);  a1sc(c32); a1vc(c32);
  a1sc(f64); a1vc(f64);  a1sc(c64); a1vc(c64);

#undef a1sc
#undef a1vc

#define a1sop(t, o)     a.def(""#t#o, I{ i.dpush(i.dpop().template at<t>(0) o i.dpop().template at<t>(0)); })
#define a1suop(t, o, n) a.def(""#t#n, I{ i.dpush(o i.dpop().template at<t>(0)); })

#define a1vop(t, o) a.def(""#t "s"#o, I{                                \
      πhnf f{i.h, 3};                                                   \
      typedef decltype(std::declval<t>() o std::declval<t>()) r;        \
      i9 x = i.pop(); f(&x);                                            \
      i9 y = i.pop(); f(&y);                                            \
      if (x.vn() == 1)                                                  \
      { i9 z = i.h << o9vec<r>{y.vn()}; f(&z);                          \
        let xa = x.template at<t>(0);                                   \
        let nb = y.vn();                                                \
        for (uN i = 0; i < nb; ++i)                                     \
          z.template set<r>(i, xa o y.template at<t>(i));               \
        i << z; }                                                       \
      else if (y.vn() == 1)                                             \
      { i9 z = i.h << o9vec<r>{x.vn()}; f(&z);                          \
        let xb = y.template at<t>(0);                                   \
        let na = x.vn();                                                \
        for (uN i = 0; i < na; ++i)                                     \
          z.template set<r>(i, x.template at<t>(i) o xb);               \
        i << z; }                                                       \
      else                                                              \
      { let k = std::min(x.vn(), y.vn());                               \
        i9 z = i.h << o9vec<r>{k}; f(&z);                               \
        for (uN i = 0; i < k; ++i)                                      \
          z.template set<r>(                                            \
            i, x.template at<t>(i) o y.template at<t>(i));              \
        i << z; }});

#define a1vuop(t, o, n) a.def(#t "s"#n, I{                      \
      πhnf f{i.h, 2};                                           \
      i9 x = i.pop(); f(&x);                                    \
      i9 y = i.h << o9vec<t>{x.vn()}; f(&y);                    \
      i << y;                                                   \
      let k = x.vn();                                           \
      for (uN i = 0; i < k; ++i)                                \
        y.template set<t>(i, o x.template at<t>(i)); });

#define a1gena(f, g) f(g, +); f(g, -); f(g, *); f(g, /);
#define a1rela(f, g) f(g, ==); f(g, !=); f(g, <); f(g, >); f(g, <=); f(g, >=);
#define a1moda(f, g) f(g, %);
#define a1bita(f, g) f(g, &); f(g, |); f(g, ^); f(g, <<); f(g, >>);

#define a1genu(f, g) f(g, -, n);
#define a1bitu(f, g) f(g, !, l); f(g, ~, ~);

#define a1fi(f, ...) f(i8,  __VA_ARGS__); f(i16, __VA_ARGS__); f(i32, __VA_ARGS__); f(i64, __VA_ARGS__); f(iN, __VA_ARGS__);
#define a1fu(f, ...) f(u8,  __VA_ARGS__); f(u16, __VA_ARGS__); f(u32, __VA_ARGS__); f(u64, __VA_ARGS__); f(uN, __VA_ARGS__);
#define a1ff(f, ...) f(f32, __VA_ARGS__); f(f64, __VA_ARGS__);
#define a1fc(f, ...) f(c32, __VA_ARGS__); f(c64, __VA_ARGS__);

  a1genu(a1fi, a1suop);  a1genu(a1fi, a1vuop);
  a1genu(a1fu, a1suop);  a1genu(a1fu, a1vuop);
  a1genu(a1ff, a1suop);  a1genu(a1ff, a1vuop);
  a1genu(a1fc, a1suop);  a1genu(a1fc, a1vuop);

  a1bitu(a1fi, a1suop);  a1bitu(a1fi, a1vuop);
  a1bitu(a1fu, a1suop);  a1bitu(a1fu, a1vuop);

  a1gena(a1fi, a1sop);  a1gena(a1fi, a1vop);
  a1gena(a1fu, a1sop);  a1gena(a1fu, a1vop);
  a1gena(a1ff, a1sop);  a1gena(a1ff, a1vop);
  a1gena(a1fc, a1sop);  a1gena(a1fc, a1vop);

  a1rela(a1fi, a1sop);  a1rela(a1fi, a1vop);
  a1rela(a1fu, a1sop);  a1rela(a1fu, a1vop);
  a1rela(a1ff, a1sop);  a1rela(a1ff, a1vop);

  a1moda(a1fi, a1sop);  a1moda(a1fi, a1vop);
  a1moda(a1fu, a1sop);  a1moda(a1fu, a1vop);

  a1bita(a1fi, a1sop);  a1bita(a1fi, a1vop);
  a1bita(a1fu, a1sop);  a1bita(a1fu, a1vop);

  // TODO: vectorize
  a1vop(b, &); a1vop(b, |); a1vop(b, ^); a1vuop(b, !, l);

#undef a1fi
#undef a1fu
#undef a1ff
#undef a1fc

#undef a1sop
#undef a1suop
#undef a1vop
#undef a1vuop

#undef a1genu
#undef a1bitu

#undef a1gena
#undef a1rela
#undef a1moda
#undef a1bita
}


void πabi1_u9_string(πabi &a)
{
  // TODO
}


void πabi1_u9_symbol(πabi &a)
{
  a .def("@:", I{ i.dpush(u9_symbol::gensym()); });
}


void πabi1_u9_fft(πabi &a)
{
  // TODO
}


void πabi1_u9_nrange(πabi &a)
{
#define a1r(t) a.def(#t ".", I{                 \
      let f = i.bpop();                         \
      let k = i.dpop().template at<t>(0);       \
      πrsf r{i};                                \
      for (t j = 0; r && j < k; ++j)            \
        i.dpush(j).run(f); });

  a1r(i8); a1r(i16); a1r(i32); a1r(i64); a1r(iN);
  a1r(u8); a1r(u16); a1r(u32); a1r(u64); a1r(uN);

#undef a1r
}


void πabi1_u9_tuple(πabi &a)
{
  a .def("t@", I{ let t = i.dpop().deref(); i << t[Sc<uN>(i.dpop())]; })
    .def("t#", I{ i.dpush(i.dpop().as_tuple().len()); })
    .def("»t", I{
        uNc k = Sc<iN>(i.dpop());
        πhnf f{i.h, 0};
        V<i9> xs; xs.reserve(k); f(&xs);
        for (uN j = 0; j < k; ++j) xs.push_back(i.dpop());
        i.dpush(xs); })
    .def("t++", I{
        let k = i9{i[0]}.as_tuple().len() + i9{i[1]}.as_tuple().len();
        πhnf f{i.h, 0};
        V<i9> xs; xs.reserve(k); f(&xs);
        for (let x : i.dpop().as_tuple()) xs.push_back(x);
        for (let x : i.dpop().as_tuple()) xs.push_back(x);
        i.dpush(xs); })
    .def("t.", I{
        πhnf f{i.h, 2};
        let  g  = i.bpop();
        i9   xs = i.dpop().as_tuple(); f(&xs);
        i9   x  = xs.first();          f(&x);
        i9   e;                        f([&]() { e = xs.next(); });
        πrsf r{i};
        while (r && x.a < e.a) { i << x; i.run(g); ++x; } })
    .def("t*", I{
        πhnf f{i.h, 2};
        let  g  = i.bpop();
        i9   xs = i.dpop().as_tuple();  f(&xs);
        i9   x  = xs.first();           f(&x);
        i9   e;                         f([&]() { e = xs.next(); });
        V<i9> ys; ys.reserve(xs.len()); f(&ys);
        πrsf r{i};
        while (r && x.a < e.a) { i << x; i.run(g); ys.push_back(i.dpop()); ++x; }
        i.dpush(ys); })
    .def("t%", I{
        πhnf f{i.h, 2};
        let  g  = i.bpop();
        i9   xs = i.dpop().as_tuple();  f(&xs);
        i9   x  = xs.first();           f(&x);
        i9   e;                         f([&]() { e = xs.next(); });
        V<i9> ys; ys.reserve(xs.len()); f(&ys);
        πrsf r{i};
        while (r && x.a < e.a) { i << x; i.run(g); if (i.dpop().b()) ys.push_back(x); ++x; }
        i.dpush(ys); })
    .def("t/++", I{
        uN k = 0;
        for (let x : i[0].as_tuple()) k += x.as_tuple().len();
        πhnf f{i.h, 0};
        V<i9> xs; xs.reserve(k); f(&xs);
        for (let x : i[0].as_tuple())
          for (let y : x.as_tuple()) xs.push_back(y);
        i.dpop();
        i.dpush(xs); });
}


void πabi1_u9_set(πabi &a)
{
  a .def("s∅", I{ i.dpush(o9t().s()); })
    .def("»s", I{
        uNc k = Sc<iN>(i.dpop());
        πhnf f{i.h, 0};
        V<i9> xs; xs.reserve(k); f(&xs);
        for (uN j = 0; j < k; ++j) xs.push_back(i.dpop());
        i9_ssort(xs.begin(), xs.end());
        i.dpush(xs);
        i[0].deref().retype(u9t::tuple, u9t::set); })
    .def("s?", I{ let s = i.dpop().deref(); i << s[i.dpop()]; })
    .def("s+", I{
        let s = i.dpop();
        let x = i.dpop();
        if (s[x]) i << s;
        else
        { πhnf f{i.h, 0};
          V<i9> xs; s.into(xs, 1); f(&xs); xs.push_back(x);
          i.dpush(xs);
          i[0].deref().retype(u9t::tuple, u9t::set); }})
    .def("s|", I{
        let b = i.dpop(); A(b.is_sord(), "s| b !is_sord: " << b);
        let a = i.dpop(); A(a.is_sord(), "s| a !is_sord: " << a);
        πhnf f{i.h, 0};
        V<i9> m; m.reserve(a.as_set().len() + b.as_set().len()); f(&m);
        i9 bx = b.first(); let be = b.next();
        i9 ax = a.first(); let ae = a.next();
        while (ax.a < ae.a && bx.a < be.a)
        { let ah = ax.h();
          let bh = bx.h();
          if      (ah < bh) { m.push_back(ax); ++ax; }
          else if (bh < ah) { m.push_back(bx); ++bx; }
          else              { m.push_back(bx); ++bx; ++ax; }}
        while (ax.a < ae.a) { m.push_back(ax); ++ax; }
        while (bx.a < be.a) { m.push_back(bx); ++bx; }
        i.dpush(m);
        i[0].deref().retype(u9t::tuple, u9t::set); })
    .def("s&", I{
        let b = i.dpop(); A(b.is_sord(), "s& b !is_sord: " << b);
        let a = i.dpop(); A(a.is_sord(), "s& a !is_sord: " << a);
        πhnf f{i.h, 0};
        V<i9> m; m.reserve(a.as_set().len() + b.as_set().len()); f(&m);
        i9 bx = b.first(); let be = b.next();
        i9 ax = a.first(); let ae = a.next();
        while (ax.a < ae.a && bx.a < be.a)
        { let ah = ax.h();
          let bh = bx.h();
          if      (ah < bh) ++ax;
          else if (bh < ah) ++bx;
          else              { m.push_back(ax); ++ax; ++bx; }}
        i.dpush(m);
        i[0].deref().retype(u9t::tuple, u9t::set); })
    .def("s.", I{
        πhnf f{i.h, 2};
        let  g  = i.bpop();
        i9   xs = i.dpop().as_tuple(); f(&xs);
        i9   x  = xs.first();          f(&x);
        i9   e;                        f([&]() { e = xs.next(); });
        πrsf r{i};
        while (r && x.a < e.a) { i << x; i.run(g); ++x; } })
    .def("s»", I{
        if (!i[0].deref().is_sord())
        { πhnf f{i.h, 2};
          V<i9> xs; i.dpop().deref().into(xs); f(&xs);
          i9_ssort(xs.begin(), xs.end());
          i.dpush(xs);
          i[0].deref().retype(u9t::tuple, u9t::set); }});
}


void πabi1_u9_map(πabi &a)
{
  a .def("m∅", I{ i.dpush(o9t().m()); })
    .def("»m", I{
        uNc k = Sc<iN>(i.dpop());
        πhnf f{i.h, 0};
        V<i9> xs; xs.reserve(k * 2); f(&xs);
        for (uN j = 0; j < k; ++j) xs.push_back(i[j]), xs.push_back(i[j + k]);
        i.drop(k * 2);
        i9_msort(xs.begin(), xs.end());
        i.dpush(xs);
        i[0].deref().retype(u9t::tuple, u9t::map); })
    .def("^m", I{
        πhnf f{i.h, 0};
        let ks = i.dpop().as_tuple();
        let vs = i.dpop().as_tuple();
        V<i9> m; m.reserve(std::min(ks.len(), vs.len())); f(&m);
        for (i9 k = ks.first(), ke = ks.next(),
                v = vs.first(), ve = vs.next();
             k.a < ke.a && v.a < ve.a;
             ++k, ++v)
          m.push_back(k), m.push_back(v);
        i9_msort(m.begin(), m.end());
        i.dpush(m);
        i[0].deref().retype(u9t::tuple, u9t::map); })
    .def("m@", I{ let m = i.dpop().deref(); i << m[i.dpop()]; })
    .def("m+", I{
        let m = i.dpop();
        let k = i.dpop();
        let v = i.dpop();
        πhnf f{i.h, 0};
        M<i9, i9> xs; m.into(xs); f(&xs); xs[k] = v;
        i.dpush(xs); })
    .def("m|", I{
        let b = i.dpop(); A(b.is_mord(), "m| b !is_mord: " << b);
        let a = i.dpop(); A(a.is_mord(), "m| a !is_mord: " << a);
        πhnf f{i.h, 0};
        V<i9> m; m.reserve(a.as_map().len() + b.as_map().len()); f(&m);
        i9 bx = b.first(); let be = b.next();
        i9 ax = a.first(); let ae = a.next();
        while (ax.a < ae.a && bx.a < be.a)
        { let ah = ax.h();
          let bh = bx.h();
          if      (ah < bh) { m.push_back(ax); m.push_back(++ax); ++ax; }
          else if (bh < ah) { m.push_back(bx); m.push_back(++bx); ++bx; }
          else              { m.push_back(bx); m.push_back(++bx); ++bx; ++ax; ++ax; }}
        while (ax.a < ae.a) { m.push_back(ax); m.push_back(++ax); ++ax; }
        while (bx.a < be.a) { m.push_back(bx); m.push_back(++bx); ++bx; }
        i.dpush(m);
        i[0].deref().retype(u9t::tuple, u9t::map); })
    .def("m&", I{
        let b = i.dpop(); A(b.is_mord(), "m& b !is_mord: " << b);
        let a = i.dpop(); A(a.is_mord(), "m& a !is_mord: " << a);
        πhnf f{i.h, 0};
        V<i9> m; m.reserve(a.as_map().len() + b.as_map().len()); f(&m);
        i9 bx = b.first(); let be = b.next();
        i9 ax = a.first(); let ae = a.next();
        while (ax.a < ae.a && bx.a < be.a)
        { let ah = ax.h();
          let bh = bx.h();
          if      (ah < bh) { ++ax; ++ax; }
          else if (bh < ah) { ++bx; ++bx; }
          else              { m.push_back(ax); m.push_back(++ax); ++ax; ++bx; ++bx; }}
        i.dpush(m);
        i[0].deref().retype(u9t::tuple, u9t::map); })
    .def("mk", I{
        let   m = i.dpop().as_map();
        uNc   k = m.len() >> 1;
        πhnf  f{i.h, 0};
        V<i9> xs; xs.reserve(k); f(&xs);
        for (let x : m.keys()) xs.push_back(x);
        i.dpush(xs); })
    .def("mv", I{
        let   m = i.dpop().as_map();
        uNc   k = m.len() >> 1;
        πhnf  f{i.h, 0};
        V<i9> xs; xs.reserve(k); f(&xs);
        for (let x : m.keys()) xs.push_back(x.next());
        i.dpush(xs); })
    .def("m.", I{
        πhnf f{i.h, 2};
        let  g  = i.bpop();
        i9   xs = i.dpop().as_map(); f(&xs);
        i9   x  = xs.first();        f(&x);
        i9   e;                      f([&]() { e = xs.next(); });
        πrsf r{i};
        while (r && x.a < e.a) { i << x; i << ++x; i.run(g); ++x; } })
    .def("m»", I{
        if (!i[0].deref().is_mord())
        { πhnf f{i.h, 2};
          V<i9> xs; i.dpop().deref().into(xs); f(&xs);
          i9_msort(xs.begin(), xs.end());
          i.dpush(xs);
          i[0].deref().retype(u9t::tuple, u9t::map); }});
}


void πabi1_u9_index(πabi &a)
{
  a .def(">i", I{
      πhnf f{i.h, 1};
      let b = Sc<u8>(i.dpop());
      i9  x = i[0].deref();
      f(&x);
      i.dpop();
      i.dpush(o9idx{x, x.is_map() ? Sc<u8>(b + 1) : b}); })
    .def("i<", I{ i << i.dpop().ivec(); })
    .def("i>", I{ i << i.dpop().icoll(); })
    .def("i#", I{ i.dpush(i.dpop().ibits()); });
}


void πabi1_u9_structure(πabi &a)
{
  a .def("(", I{ i.dpush(Sc<iN>(1)); i.spush(1); })
    .def("{", I{ i.dpush(Sc<iN>(1)); i.spush(1); })
    .def(")", I{
        let x = i[0];
        i.spop();
        uNc k = Sc<iN>(i.dpop());
        i << x;
        πhnf f{i.h, 0};
        V<i9> xs; xs.reserve(k); f(&xs);
        for (uN j = 0; j < k; ++j) xs.push_back(i[k - j - 1]);
        i.drop(k);
        i.dpush(xs); })
    .def("}", I{
        let x = i[0];
        i.spop();
        uNc k = Sc<iN>(i.dpop());
        A(!(k & 1), "{} size = " << k << " (must be even)");
        i << x;
        πhnf f{i.h, 0};
        V<i9> xs; xs.reserve(k); f(&xs);
        for (uN j = 0; j < k; ++j) xs.push_back(i[k - j - 1]);
        i.drop(k);
        i.dpush(xs);
        i[0].deref().retype(u9t::tuple, u9t::map); })
    .def(",", I{
        let x = i[0];
        i.spop();
        let k = Sc<iN>(i.dpop());
        i << x;
        i.dpush(k + 1);
        i.spush(k + 1); });
}


void πabi1_u9_quote(πabi &a)
{
  a. def("$b",   I{ i << (i.h << πo9q(i.dpop())); })
    .def("b$!!", I{ i.dpush(i9{i.dpop().data()}); })
    .def("b$", I{
        let x = i.dpop();
        let j = i9{x.data()};
        i.dpush(j.verify(x.size()) == i9v::valid ? j : i9_none()); });
}


void πabi1_λ(πabi &a)
{
  // NOTE: λ: and λ∷ have subtle GC semantics because the lambda body
  // doesn't run synchronously, so the .dpush() may trigger a GC that
  // moves the init arg. As a result, we need to make sure it's bound
  // into a native frame whose lifetime extends until the λ has pushed
  // it into its own interpreter data stack.
  a .def("λ:", I{
        let f = new πhnf{i.h, 1};
        let b = i.bpop();
        let x = new i9{i[0]};
        (*f)(x); i.dpop();
        i.dpush(i.f.l.c([=, &i]()
          { πint j(i);
            j << *x; delete x; delete f;
            j.run(b); return 0; })); })
    .def("λ∷", I{
        let f = new πhnf{i.h, 1};
        let b = i.bpop();
        let x = new i9{i[0]};
        (*f)(x); i.dpop();
        i.dpush(i.f.l.c([=, &i]()
          { πint j{i, SP<πhgs>{new πhgs{i.h, i.g->s}}};
            j << *x; delete x; delete f;
            j.run(b); return 0; })); })
    .def("λ_", I{ i.f.l.x(Sc<λi>(i.dpop())); })
    .def("λy", I{ i.f.l.y(λs::Y); })
    .def("Θd", I{ i.f.Θ(now() + Sc<uN>(i.dpop()) * 1ns); });
}


void πabi1_η(πabi &a)
{
  a .def("η:", I{ i.dpush(o9ptr(new πhη(i.h))); })
    .def("η_", I{ let y = i.dpop().template ptr<πhη>(); delete y; })
    .def("η=", I{
        let y = i.dpop().template ptr<πhη>();
        let k = i.dpop();
        let v = i.dpop();
        if (v.ω()) y->x(k);
        else       (*y)[k] = v.a; })
    .def("η@", I{
        let y = i.dpop().template ptr<πhη>();
        let k = i.dpop();
        i << (*y)[k]; })
    .def("η?", I{
        let y = i.dpop().template ptr<πhη>();
        let k = i.dpop();
        i.dpush(y->i(k)); });
}


void πabi1_φ(πabi &a)
{
  a .def("φ:", I{
        let fs = Sc<u8>(i.dpop());
        let ζb = Sc<u8>(i.dpop());
        i.dpush(o9ptr(new φ(i.f.l, ζb, fs))); })
    .def("φ_", I{
        let f = i.dpop().template ptr<φ>();
        delete f; })
    .def("φ|", I{
        let a = i.dpop().template ptr<φ>();
        let b = i.dpop().template ptr<φ>();
        (*a)(*b); })
    .def("φ<>",  I{
        let b = i.dpop().template ptr<φ>();
        let a = i.dpop().template ptr<φ>();
        let x = **a;
        i.dpush(x.exists() && *b << x);
        ++*a; })
    .def("φ<>=", I{
        let b = i.dpop().template ptr<φ>();
        let a = i.dpop().template ptr<φ>();
        let x = **a;
        i.dpush(x.exists() && (*b <<= x));
        ++*a; })
    .def("φ<",   I{ let f = i.dpop().template ptr<φ>(); let x = i.dpop(); A(!x.flagged(), "φ< flagged");  i.dpush(o9(*f <<  x)); })
    .def("φ<=",  I{ let f = i.dpop().template ptr<φ>(); let x = i.dpop(); A(!x.flagged(), "φ<= flagged"); i.dpush(o9(*f <<= x)); })
    .def("φ*",   I{ i.dpush(o9(**i.dpop().template ptr<φ>())); })
    .def("φ++",  I{ ++*i.dpop().template ptr<φ>(); })
    .def("φ<?",  I{ i.dpush(o9(i.dpop().template ptr<φ>()->wi())); })
    .def("φ>?",  I{ i.dpush(o9(i.dpop().template ptr<φ>()->ri())); })
    .def("φω?",  I{ i.dpush(o9(i.dpop().template ptr<φ>()->ωi())); })
    .def("φ<ω",  I{ i.dpop().template ptr<φ>()->wω(); })
    .def("φ>ω",  I{ i.dpop().template ptr<φ>()->rω(); })
    .def("φω",   I{ i.dpop().template ptr<φ>()->ω(); })
    .def("φ.",   I{
        let b = i.bpop();
        let f = i.dpop().template ptr<φ>();
        let r = πrsf(i);
        for (let x : *f)
          if (!r) break;
          else    i.dpush(x).run(b); });
}


#if τdebug
void πabi1_debug(πabi &a)
{
  a .def(":gc",    I{ i.h.gc(); })
    .def(":gH",    I{ std::cerr << i.h.gΘ << std::endl; })
    .def(":gL",    I{ std::cerr << i.h.Σa << "/" << i.h.lss0 << ": " << i.h.gl << std::endl; })
    .def(":src",   I{ std::cerr << i.p << std::endl; })
    .def(":int",   I{ std::cerr << i << std::endl; })
    .def(":dn",    I{ std::cerr << "|d| = " << i.dv->size() << std::endl; })
    .def(":spush", I{ i.spush(); })
    .def(":spop",  I{ i.spop(); })
    .def(":cout", I{
        let x = i.dpop();
        u9strings(x.type());
        std::cout << Stv{Rc<chc*>(x.data()), x.size()}; })
    .def(":cerr", I{
        let x = i.dpop();
        std::cerr << Stv{Rc<chc*>(x.data()), x.size()}; })
    .def(":pgm", I{ std::cerr << *i.p << std::endl; })

    // NOTE: these are used by unit tests, so they need to write to
    // stdout since that's what lazytest expects
    .def(":data",  I{
        for (uN j = 0; j < i.size(); ++j)
          std::cout << "[" << j << "]\t" << i[j] << std::endl; })
    .def(":out",   I{ std::cout << i.dpop() << std::endl; })
    .def(":outn",  I{ std::cout << i.dpop(); })
    .def(":err",   I{ std::cerr << i.dpop() << std::endl; })
    .def(":errn",  I{ std::cerr << i.dpop(); });
}
#endif


void πabi1(πabi &a)
{
  πabi1_const(a);
  πabi1_blocks(a);
  πabi1_stack(a);
  πabi1_frame(a);
  πabi1_global(a);
  πabi1_control(a);
  πabi1_quote(a);

  πabi1_u9_general(a);
  πabi1_u9_vector(a);
  πabi1_u9_number(a);
  πabi1_u9_string(a);
  πabi1_u9_symbol(a);
  πabi1_u9_fft(a);
  πabi1_u9_nrange(a);
  πabi1_u9_tuple(a);
  πabi1_u9_set(a);
  πabi1_u9_map(a);
  πabi1_u9_index(a);
  πabi1_u9_structure(a);
  πabi1_u9_quote(a);

  πabi1_λ(a);
  πabi1_η(a);
  πabi1_φ(a);

# if τdebug
  πabi1_debug(a);
# endif
}


πabi const &πabi1()
{
  static πabi a;
  static bool i = false;
  if (!i) πabi1(a);
  i = true;
  return a;
}


}

#include "pi-abi-end.hh"


#endif
