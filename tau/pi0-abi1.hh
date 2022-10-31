#ifndef τπ0abi1_h
#define τπ0abi1_h


#include "../dep/pffft.h"

#include "utf9.hh"
#include "pi0-types.hh"
#include "pi0-gc.hh"
#include "pi0-abi.hh"
#include "pi0-int.hh"

#include "begin.hh"

namespace τ
{


#define I [](π0int &i, π0fa n)


void π0abi1_blocks(π0abi &a)
{
  a .def("[",  I{ i.dpush(Sc<π0bi>(i.r.back())); i.r.back() += n; })
    .def("]",  I{ i.r.pop_back(); })
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
    .def("%",  I
         { let a = i[0];
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
    .def("&=",  I
         { let v = Sc<uN>(i.dpop());
           i.fi(v) = i.pop(); });
}


void π0abi1_control(π0abi &a)
{
  a .def(".",  I{ i.r.push_back(Sc<π0bi>(i.dpop())); })
    .def("?.", I
         { π0bi e = i.dpop();
           π0bi t = i.dpop();
           let  c = i.dpop().template at<bool>(0);
           i.run(c ? t : e); })
    .def("?!", I
         { π0bi c = i.dpop();
           π0bi b = i.dpop();
         loop:
           i.run(c);
           if (i.dpop().template at<bool>(0)) { i.run(b); goto loop; }})
    .def(".^", I{ i.r.resize(i.r.size() - n - 1); });
}


void π0abi1_quote(π0abi &a)
{
  // NOTE: in a later ABI, these should probably be "copy from static";
  // but for now it's useful to unpack args from bytecode
  a .def("i8",  I{ i.dpush(Sc<i8>(n)); })
    .def("i16", I{ i.dpush(Sc<i16>(n)); })
    .def("i32", I{ i.dpush(Sc<i32>(n)); })
    .def("i64", I{ i.dpush(Sc<i64>(n)); })

    .def("utf8", I{ i.dpush(i9{Cc<ζp>(i.p.q.data() + n)}); })
    .def("sym",  I{ i.dpush(i9{Cc<ζp>(i.p.q.data() + n)}); });
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
        i9 a = i.dpop(); f(&a);                                        \
        i9 b = i.dpop(); f(&b);                                        \
        i9 c  = i.h << o9vec<t>{a.vn() + b.vn()}; f(&c);               \
        for (uN j = 0; j < a.vn(); ++j)                                \
          c.template set<t>(j, a.template at<t>(j));                   \
        for (uN j = 0; j < b.vn(); ++j)                                \
          c.template set<t>(j + a.vn(), b.template at<t>(j));          \
        i << c; })                                                     \
    .def(#t "s.", I{                                                   \
        π0hnf f{i.h, 1};                                               \
        let g = i.dpop();                                              \
        i9 xs = i.dpop(); f(&xs);                                      \
        for (uN j = 0; j < xs.vn(); ++j)                               \
        { i.dpush(xs.template at<t>(j));                               \
          i.run(g); } })

  a1vfns(i8);  a1vfns(i16); a1vfns(i32); a1vfns(i64);
  a1vfns(u8);  a1vfns(u16); a1vfns(u32); a1vfns(u64);
  a1vfns(f32); a1vfns(f64);
  a1vfns(c32); a1vfns(c64);

#undef a1vfns
}


void π0abi1_u9_number(π0abi &a)
{
#define a1fi(f, ...) f(i8,  __VA_ARGS__); f(i16, __VA_ARGS__); f(i32, __VA_ARGS__); f(i64, __VA_ARGS__);
#define a1fu(f, ...) f(u8,  __VA_ARGS__); f(u16, __VA_ARGS__); f(u32, __VA_ARGS__); f(u64, __VA_ARGS__);
#define a1ff(f, ...) f(f32, __VA_ARGS__); f(f64, __VA_ARGS__);
#define a1fc(f, ...) f(c32, __VA_ARGS__); f(c64, __VA_ARGS__);

#define a1sc(t, u) a.def(#t  ">"#u, I{ i.dpush(Sc<u>(i.dpop().template at<t>(0))); });
#define a1vc(t, u) a.def(#t "s>"#u, I{                          \
      π0hnf f{i.h, 2};                                          \
      i9 a = i.pop(); f(&a);                                    \
      i9 b = i.h << o9vec<u>{a.vn()}; f(&b);                    \
      i << b;                                                   \
      for (uN i = 0; i < a.vn(); ++i)                           \
        b.template set<u>(i, a.template at<t>(i)); });

#define a1cfi(f, g) f(g, i8);  f(g, i16); f(g, i32); f(g, i64);
#define a1cfu(f, g) f(g, u8);  f(g, u16); f(g, u32); f(g, u64);
#define a1cff(f, g) f(g, f32); f(g, f64);
#define a1cfc(f, g) f(g, c32); f(g, c64);

  a1cfi(a1fi, a1sc);  a1cfi(a1fu, a1sc);  a1cfi(a1ff, a1sc);
  a1cfu(a1fi, a1sc);  a1cfu(a1fu, a1sc);  a1cfu(a1ff, a1sc);
  a1cff(a1fi, a1sc);  a1cff(a1fu, a1sc);  a1cff(a1ff, a1sc);
  a1cfc(a1fi, a1sc);  a1cfc(a1fu, a1sc);  a1cfc(a1ff, a1sc);

  a1cfi(a1fi, a1vc);  a1cfi(a1fu, a1vc);  a1cfi(a1ff, a1vc);
  a1cfu(a1fi, a1vc);  a1cfu(a1fu, a1vc);  a1cfu(a1ff, a1vc);
  a1cff(a1fi, a1vc);  a1cff(a1fu, a1vc);  a1cff(a1ff, a1vc);
  a1cfc(a1fi, a1vc);  a1cfc(a1fu, a1vc);  a1cfc(a1ff, a1vc);

#undef a1cfi
#undef a1cfu
#undef a1cff
#undef a1cfc

#undef a1sc
#undef a1vc

#define a1sop(t, o)     a.def(""#t#o, I{ i.dpush(i.dpop().template at<t>(0) o i.dpop().template at<t>(0)); })
#define a1suop(t, o, n) a.def(""#t#n, I{ i.dpush(o i.dpop().template at<t>(0)); })

#define a1vop(t, o) a.def(""#t "s"#o, I{                                \
      π0hnf f{i.h, 3};                                                  \
      typedef decltype(std::declval<t>() o std::declval<t>()) r;        \
      i9 a = i.pop(); f(&a);                                            \
      i9 b = i.pop(); f(&b);                                            \
      if (a.vn() == 1)                                                  \
      { i9 c = i.h << o9vec<r>{b.vn()}; f(&c);                          \
        let xa = a.template at<t>(0);                                   \
        for (uN i = 0; i < b.vn(); ++i)                                 \
          c.template set<r>(i, xa o b.template at<t>(i));               \
        i << c; }                                                       \
      else if (b.vn() == 1)                                             \
      { i9 c = i.h << o9vec<r>{a.vn()}; f(&c);                          \
        let xb = b.template at<t>(0);                                   \
        for (uN i = 0; i < a.vn(); ++i)                                 \
          c.template set<r>(i, a.template at<t>(i) o xb);               \
        i << c; }                                                       \
      else                                                              \
      { let k = std::min(a.vn(), b.vn());                               \
        i9 c = i.h << o9vec<r>{k}; f(&c);                               \
        for (uN i = 0; i < k; ++i)                                      \
          c.template set<r>(                                            \
            i, a.template at<t>(i) o b.template at<t>(i));              \
        i << c; }});

#define a1vuop(t, o, n) a.def(#t "s"#n, I{                      \
      π0hnf f{i.h, 2};                                          \
      i9 a = i.pop(); f(&a);                                    \
      i9 b = i.h << o9vec<t>{a.vn()}; f(&b);                    \
      i << b;                                                   \
      for (uN i = 0; i < a.vn(); ++i)                           \
        b.template set<t>(i, o a.template at<t>(i)); });

#define a1gena(f, g) f(g, +); f(g, -); f(g, *); f(g, /);
#define a1rela(f, g) f(g, ==); f(g, !=); f(g, <); f(g, >); f(g, <=); f(g, >=);
#define a1moda(f, g) f(g, %);
#define a1bita(f, g) f(g, &); f(g, |); f(g, ^); f(g, <<); f(g, >>);

#define a1genu(f, g) f(g, -, n);
#define a1bitu(f, g) f(g, !, l); f(g, ~, ~);

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

#undef a1sop
#undef a1suop
#undef a1vop
#undef a1vuop

#undef a1fi
#undef a1fu
#undef a1ff
#undef a1fc

#undef a1genu
#undef a1bitu

#undef a1gena
#undef a1rela
#undef a1moda
#undef a1bita
}


void π0abi1_u9_fft(π0abi &a)
{
  // TODO
}


void π0abi1_u9_nrange(π0abi &a)
{
#define a1r(t) a.def(#t ".", I{                 \
      π0bi f = i.dpop();                        \
      let  k = i.dpop().template at<t>(0);      \
      for (t j = 0; j < k; ++j)                 \
        i.dpush(j).run(f); });

  a1r(i8); a1r(i16); a1r(i32); a1r(i64);
  a1r(u8); a1r(u16); a1r(u32); a1r(u64);

#undef a1r
}


void π0abi1_u9_tuple(π0abi &a)
{
  a .def(">>t", I{
      uNc k = Sc<iN>(i.dpop());
      π0hnf f{i.h, 0};
      V<i9> xs; xs.reserve(k); f(&xs);
      for (uN j = 0; j < k; ++j) xs.push_back(i.pop());
      i.dpush(xs); })
    .def("t@", I{ uNc k = Sc<iN>(i.dpop()); i.dpush(i.dpop()[k]); })
    .def("t#", I{ i.dpush(i.dpop().len()); })
    .def("t++", I{
        let k = i9{i[0]}.len() + i9{i[1]}.len();
        π0hnf f{i.h, 0};
        V<i9> xs; xs.reserve(k); f(&xs);
        for (let x : i.dpop()) xs.push_back(x);
        for (let x : i.dpop()) xs.push_back(x);
        i.dpush(xs); })
    .def("t.", I{
        π0hnf f{i.h, 2};
        π0bi  g  = i.dpop();
        i9    xs = i.dpop();   f(&xs);
        i9    x  = xs.first(); f(&x);
        i9    e;               f([&]() { e = xs.next(); });
        while (x.a < e.a) { i << x; i.run(g); ++x; } });
}


void π0abi1_u9_structure(π0abi &a)
{
  a .def("(", I{ i.dpush(Sc<iN>(1)); i.spush(1); })
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
    .def(",", I{
        let x = i[0];
        i.spop();
        let k = Sc<iN>(i.dpop());
        i << x;
        i.dpush(k + 1);
        i.spush(k + 1); });
}


#if τdebug
void π0abi1_debug(π0abi &a)
{
  a .def(":gc",    I{ i.h.gc(); })
    .def(":gH",    I{ std::cout << i.h.gΘ << std::endl; })
    .def(":gL",    I{ std::cout << i.h.lss0 << ": " << i.h.gl << std::endl; })
    .def(":src",   I{ std::cout << i.p << std::endl; })
    .def(":int",   I{ std::cout << i << std::endl; })
    .def(":out",   I{ std::cout << i[0] << std::endl; })
    .def(":spush", I{ i.spush(); })
    .def(":spop",  I{ i.spop(); })
    .def(":data", I
         { for (uN j = 0; j < i.size(); ++j)
             std::cout << "[" << j << "]\t" << i[j] << std::endl; });
}
#endif


#undef I


π0abi const &π0abi1()
{
  static π0abi a;
  static bool  i = false;

  if (i) return a;

  π0abi1_blocks(a);
  π0abi1_stack(a);
  π0abi1_frame(a);
  π0abi1_control(a);
  π0abi1_quote(a);
  π0abi1_u9_general(a);
  π0abi1_u9_vector(a);
  π0abi1_u9_number(a);
  π0abi1_u9_fft(a);
  π0abi1_u9_nrange(a);
  π0abi1_u9_tuple(a);
  π0abi1_u9_structure(a);

# if τdebug
  π0abi1_debug(a);
# endif

  i = true;
  return a;
}


}

#include "end.hh"


#endif
