#ifndef τπ0abi1_h
#define τπ0abi1_h


#include "arch.hh"

#include "../dep/pffft.h"

#include "utf9-i9.hh"
#include "utf9-types.hh"
#include "utf9.hh"

#include "pi0-types.hh"
#include "pi0-gc.hh"
#include "pi0-abi.hh"
#include "pi0-int.hh"

#include "Phi.hh"


/*
#include "xi/basics.hh"
#include "xi/linux/xframe-xcbio.hh"

#if τplatform == τplatform_linux
# include "xi/linux/io.hh"
# include "xi/linux/tcp.hh"
#endif
*/


#include "begin.hh"

namespace τ
{


#define I [](π0int &i, π0fa n)


void π0abi1_const(π0abi &a)
{
  a .def("t", I{ i.dpush(true);  })
    .def("f", I{ i.dpush(false); });
}


void π0abi1_blocks(π0abi &a)
{
  a .def("[",  I{ i.dpush(Sc<π0bi>(i.rs.back())); i.rs.back() += n; })
    .def("]",  I{ i.rs.pop_back(); })
    .def("[|", I{ i.fpush(n); })
    .def("|]", I{ i.fpop(); });
}


void π0abi1_stack(π0abi &a)
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


void π0abi1_frame(π0abi &a)
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


void π0abi1_global(π0abi &a)
{
  a .def("%@", I{ let k = i.dpop(); let v = i.gs()[k]; A(i.h(v).exists(), "undefined global " << k); i << v; })
    .def("%=", I{ let s = i.dpop(); i.gs()[s] = i.dpop(); })
    .def("%_", I{ i.gs().x(i.dpop()); })
    .def("%?", I{ i.dpush(i.gs().i(i.dpop())); });
}


void π0abi1_control(π0abi &a)
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
      loop:
        i.run(c);
        if (i.dpop().b()) { i.run(b); goto loop; }})
    .def("!!", I{ let b = i.bpop(); while (1) i.run(b); })

    // FIXME: this is broken with natives
    .def(".^", I{ i.rs.resize(i.rs.size() - n - 1); });
}


void π0abi1_quote(π0abi &a)
{
  // NOTE: in a later ABI, these should probably be "copy from static";
  // but for now it's useful to unpack args from bytecode
  a .def("i8",  I{ i.dpush(Sc<i8>(n)); })
    .def("i16", I{ i.dpush(Sc<i16>(n)); })
    .def("i32", I{ i.dpush(Sc<i32>(n)); })
    .def("i64", I{ i.dpush(Sc<i64>(n)); })

    // TODO: reference these, don't copy (since they're immutable)
    .def("utf8", I{ i.dpush(i9{Cc<ζp>(i.p->q.data() + n)}); })
    .def("sym",  I{ i.dpush(i9{Cc<ζp>(i.p->q.data() + n)}); });
}


void π0abi1_u9_general(π0abi &a)
{
  a .def("?e", I{ i.dpush(i.dpop().exists()); })
    .def("?t", I{ i.dpush(u9typesyms.at(i.dpop().type())); })
    .def("?f", I{ i.dpush(i.dpop().flagged()); })
    .def("?s", I{ i.dpush(i.dpop().size()); })
    .def("?S", I{ i.dpush(i.dpop().osize()); })
    .def(":f", I{
        if (i[0].flagged())
          i << (i.h << π0o9f{i.pop()}); });
}


void π0abi1_u9_vector(π0abi &a)
{
#define a1vfns(t)                                                      \
  a .def(">>" #t "s", I{                                               \
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
        π0hnf f{i.h, 2};                                               \
        uNc sn = Sc<iN>(i.dpop());                                     \
        uNc si = Sc<iN>(i.dpop());                                     \
        i9 s = i.dpop();            f(&s);                             \
        i9 r = i.h << o9vec<t>{sn}; f(&r);                             \
        for (uN j = si; j < sn; ++j)                                   \
          r.template set<t>(j - si, s.template at<t>(j));              \
        i << r; })                                                     \
    .def(#t "s++", I{                                                  \
        π0hnf f{i.h, 3};                                               \
        i9 x = i.dpop(); f(&x);                                        \
        i9 y = i.dpop(); f(&y);                                        \
        i9 z  = i.h << o9vec<t>{x.vn() + y.vn()}; f(&z);               \
        for (uN j = 0; j < x.vn(); ++j)                                \
          z.template set<t>(j, x.template at<t>(j));                   \
        for (uN j = 0; j < y.vn(); ++j)                                \
          z.template set<t>(j + x.vn(), y.template at<t>(j));          \
        i << z; })                                                     \
    .def(#t "s.", I{                                                   \
        π0hnf f{i.h, 1};                                               \
        let g = i.bpop();                                              \
        i9 xs = i.dpop(); f(&xs);                                      \
        for (uN j = 0; j < xs.vn(); ++j)                               \
        { i.dpush(xs.template at<t>(j));                               \
          i.run(g); } })

  typedef bool b; a1vfns(b);

  a1vfns(i8);  a1vfns(i16); a1vfns(i32); a1vfns(i64);
  a1vfns(u8);  a1vfns(u16); a1vfns(u32); a1vfns(u64);
  a1vfns(f32); a1vfns(f64);
  a1vfns(c32); a1vfns(c64);

#undef a1vfns
}


void π0abi1_u9_number(π0abi &a)
{
#define a1sc(t) a.def(">"#t,    I{ i.dpush(i.dpop().template at<t>(0)); });
#define a1vc(t) a.def(">"#t"s", I{                              \
      π0hnf f{i.h, 2};                                          \
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
  a1sc(i32); a1vc(i32);  a1sc(u32); a1vc(u32);
  a1sc(i64); a1vc(i64);  a1sc(u64); a1vc(u64);

  a1sc(f32); a1vc(f32);  a1sc(c32); a1vc(c32);
  a1sc(f64); a1vc(f64);  a1sc(c64); a1vc(c64);

#undef a1sc
#undef a1vc

#define a1sop(t, o)     a.def(""#t#o, I{ i.dpush(i.dpop().template at<t>(0) o i.dpop().template at<t>(0)); })
#define a1suop(t, o, n) a.def(""#t#n, I{ i.dpush(o i.dpop().template at<t>(0)); })

#define a1vop(t, o) a.def(""#t "s"#o, I{                                \
      π0hnf f{i.h, 3};                                                  \
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
      π0hnf f{i.h, 2};                                          \
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

#define a1fi(f, ...) f(i8,  __VA_ARGS__); f(i16, __VA_ARGS__); f(i32, __VA_ARGS__); f(i64, __VA_ARGS__);
#define a1fu(f, ...) f(u8,  __VA_ARGS__); f(u16, __VA_ARGS__); f(u32, __VA_ARGS__); f(u64, __VA_ARGS__);
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


void π0abi1_u9_string(π0abi &a)
{
  // TODO
}


void π0abi1_u9_fft(π0abi &a)
{
  // TODO
}


void π0abi1_u9_nrange(π0abi &a)
{
#define a1r(t) a.def(#t ".", I{                 \
      let f = i.bpop();                         \
      let k = i.dpop().template at<t>(0);       \
      for (t j = 0; j < k; ++j)                 \
        i.dpush(j).run(f); });

  a1r(i8); a1r(i16); a1r(i32); a1r(i64);
  a1r(u8); a1r(u16); a1r(u32); a1r(u64);

#undef a1r
}


void π0abi1_u9_tuple(π0abi &a)
{
  a .def("t@", I{ let k = Sc<uN>(i.dpop()); i.dpush(i.dpop()[k]); })
    .def("t#", I{ i.dpush(i.dpop().len()); })
    .def(">>t", I{
        uNc k = Sc<iN>(i.dpop());
        π0hnf f{i.h, 0};
        V<i9> xs; xs.reserve(k); f(&xs);
        for (uN j = 0; j < k; ++j) xs.push_back(i.pop());
        i.dpush(xs); })
    .def("t++", I{
        let k = i9{i[0]}.len() + i9{i[1]}.len();
        π0hnf f{i.h, 0};
        V<i9> xs; xs.reserve(k); f(&xs);
        for (let x : i.dpop()) xs.push_back(x);
        for (let x : i.dpop()) xs.push_back(x);
        i.dpush(xs); })
    .def("t.", I{
        π0hnf f{i.h, 2};
        let   g  = i.bpop();
        i9    xs = i.dpop();   f(&xs);
        i9    x  = xs.first(); f(&x);
        i9    e;               f([&]() { e = xs.next(); });
        while (x.a < e.a) { i << x; i.run(g); ++x; } });
}


void π0abi1_u9_structure(π0abi &a)
{
  a .def("(", I{ i.dpush(Sc<iN>(1)); i.spush(1); })
    .def("{", I{ i.dpush(Sc<iN>(1)); i.spush(1); })
    .def(")", I{
        let x = i[0];
        i.spop();
        uNc k = Sc<iN>(i.dpop());
        i << x;
        π0hnf f{i.h, 0};
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
        π0hnf f{i.h, 0};
        V<i9> xs; xs.reserve(k); f(&xs);
        for (uN j = 0; j < k; ++j) xs.push_back(i[k - j - 1]);
        i.drop(k);
        i.dpush(xs);
        i.h(i[0]).retype(u9t::tuple, u9t::map); })
    .def(",", I{
        let x = i[0];
        i.spop();
        let k = Sc<iN>(i.dpop());
        i << x;
        i.dpush(k + 1);
        i.spush(k + 1); });
}


void π0abi1_u9_quote(π0abi &a)
{
  a. def("$b",   I{ i << (i.h << π0o9q(i.dpop())); })
    .def("b$!!", I{ i.dpush(i9{i.dpop().data()}); })
    .def("b$", I{
        let x = i.dpop();
        let j = i9{x.data()};
        i.dpush(j.verify(x.size()) == i9v::valid ? j : i9_none()); });
}


void π0abi1_γ(π0abi &a)
{
  a .def("λ:", I{
        let b = i.bpop();
        i.dpush(i.f.l.c([=, &i]() { π0int(i).run(b); return 0; })); })
    .def("λ_", I{ i.f.l.x(Sc<λi>(i.dpop())); })
    .def("λy", I{ i.f.l.y(λs::Y); })
    .def("Θd", I{ i.f.Θ(now() + Sc<uN>(i.dpop()) * 1ns); });
}


void π0abi1_φ(π0abi &a)
{
  a .def("φ:", I{
        let fs = Sc<u8>(i.dpop());
        let ζb = Sc<u8>(i.dpop());
        i.dpush(o9ptr(new γφ(i.f.l, ζb, fs))); })
    .def("φ_", I{
        let f = i.dpop().template ptr<γφ>();
        delete f; })
    .def("φ|", I{
        let a = i.dpop().template ptr<γφ>();
        let b = i.dpop().template ptr<γφ>();
        (*a)(*b); })
    .def("φ<>",  I{
        let b = i.dpop().template ptr<γφ>();
        let a = i.dpop().template ptr<γφ>();
        let x = **a;
        i.dpush(x.exists() && *b << x);
        ++*a; })
    .def("φ<>=", I{
        let b = i.dpop().template ptr<γφ>();
        let a = i.dpop().template ptr<γφ>();
        let x = **a;
        i.dpush(x.exists() && (*b <<= x));
        ++*a; })
    .def("φ<",   I{ let f = i.dpop().template ptr<γφ>(); let x = i.dpop(); A(!x.flagged(), "φ< flagged");  i.dpush(o9(*f <<  x)); })
    .def("φ<=",  I{ let f = i.dpop().template ptr<γφ>(); let x = i.dpop(); A(!x.flagged(), "φ<= flagged"); i.dpush(o9(*f <<= x)); })
    .def("φ*",   I{ i.dpush(o9(**i.dpop().template ptr<γφ>())); })
    .def("φ++",  I{ ++*i.dpop().template ptr<γφ>(); })
    .def("φ<?",  I{ i.dpush(o9(i.dpop().template ptr<γφ>()->wi())); })
    .def("φ>?",  I{ i.dpush(o9(i.dpop().template ptr<γφ>()->ri())); })
    .def("φω?",  I{ i.dpush(o9(i.dpop().template ptr<γφ>()->ωi())); })
    .def("φ<ω",  I{ i.dpop().template ptr<γφ>()->wω(); })
    .def("φ>ω",  I{ i.dpop().template ptr<γφ>()->rω(); })
    .def("φω",   I{ i.dpop().template ptr<γφ>()->ω(); })
    .def("φ.",   I{
        let b = i.bpop();
        let f = i.dpop().template ptr<γφ>();
        for (let x : *f) i.dpush(x).run(b); });
}


#if τdebug
void π0abi1_debug(π0abi &a)
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
        std::cout << Stv{Rc<chc*>(x.data()), x.size()}; })
    .def(":cerr", I{
        let x = i.dpop();
        std::cerr << Stv{Rc<chc*>(x.data()), x.size()}; })

    // NOTE: these are used by unit tests, so they need to write to
    // stdout since that's what lazytest expects
    .def(":data",  I{
        for (uN j = 0; j < i.size(); ++j)
          std::cout << "[" << j << "]\t" << i[j] << std::endl; })
    .def(":out",   I{ std::cout << i[0] << std::endl; });
}
#endif


#undef I


π0abi const &π0abi1()
{
  static π0abi a;
  static bool  i = false;

  if (i) return a;

  π0abi1_const(a);
  π0abi1_blocks(a);
  π0abi1_stack(a);
  π0abi1_frame(a);
  π0abi1_global(a);
  π0abi1_control(a);
  π0abi1_quote(a);

  π0abi1_u9_general(a);
  π0abi1_u9_vector(a);
  π0abi1_u9_number(a);
  π0abi1_u9_string(a);
  π0abi1_u9_fft(a);
  π0abi1_u9_nrange(a);
  π0abi1_u9_tuple(a);
  π0abi1_u9_structure(a);
  π0abi1_u9_quote(a);

  π0abi1_γ(a);
  π0abi1_φ(a);

# if τdebug
  π0abi1_debug(a);
# endif

  i = true;
  return a;
}


}

#include "end.hh"


#endif
