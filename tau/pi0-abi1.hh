#ifndef τπ0abi1_h
#define τπ0abi1_h


#include "utf9.hh"
#include "pi0-types.hh"
#include "pi0-gc.hh"
#include "pi0-abi.hh"
#include "pi0-int.hh"

#include "pi0-begin.hh"

namespace τ
{


#define I [](π0T(π0int) &i, π0fa n)

π0TG void π0abi1_blocks(π0T(π0abi) &a)
{
  a .def("[",  I{ i.dpush(Sc<π0bi>(i.r.back())); i.r.back() += n; })
    .def("]",  I{ i.r.pop_back(); })
    .def("[|", I{ i.fpush(n); })
    .def("|]", I{ i.fpop(); });
}


π0TG void π0abi1_stack(π0T(π0abi) &a)
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


π0TG void π0abi1_frame(π0T(π0abi) &a)
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


π0TG void π0abi1_control(π0T(π0abi) &a)
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


π0TG void π0abi1_quote(π0T(π0abi) &a)
{
  // NOTE: in a later ABI, these should probably be "copy from static";
  // but for now it's useful to unpack args from bytecode
  a .def("i8",  I{ i.dpush(Sc<i8>(n)); })
    .def("i16", I{ i.dpush(Sc<i16>(n)); })
    .def("i32", I{ i.dpush(Sc<i32>(n)); })
    .def("i64", I{ i.dpush(Sc<i64>(n)); })

    .def("utf8", I{ i.dpush(i9{const_cast<ζp>(i.p.q.data() + n)}); })
    .def("sym",  I{ i.dpush(i9{const_cast<ζp>(i.p.q.data() + n)}); });
}


π0TG void π0abi1_number(π0T(π0abi) &a)
{
#define a1sc(t, u) a.def(#t  ">"#u, I{ i.dpush(Sc<u>(i.dpop().template at<t>(0))); });
#define a1vc(t, u) a.def(#t "s>"#u, I{                  \
    π0T(π0hnf) f{i.h, 2};                               \
    auto &a = f << i.pop();                             \
    auto &b = f << (i.h << o9vec<u>{i.h[a].vn()});      \
    i << b;                                             \
    let ia = i.h[a];                                    \
    let ib = i.h[b];                                    \
    for (uN i = 0; i < ia.vn(); ++i)                    \
      ib.template set<u>(i, ia.template at<t>(i)); });

#define a1sop(t, o) a.def(""#t#o, I{ i.dpush(i.dpop().template at<t>(0) o i.dpop().template at<t>(0)); })

#define a1vop(t, o) a.def(""#t "s"#o, I{                                \
      π0T(π0hnf) f{i.h, 3};                                             \
      typedef decltype(std::declval<t>() o std::declval<t>()) r;        \
      auto &a = f << i.pop();                                           \
      auto &b = f << i.pop();                                           \
      if (i.h[a].vn() == 1)                                             \
      { auto &c = f << (i.h << o9vec<r>{i.h[b].vn()});                  \
        let b_ = i.h[b];                                                \
        let c_ = i.h[c];                                                \
        let xa = i.h[a].template at<t>(0);                              \
        for (uN i = 0; i < b_.vn(); ++i)                                \
          c_.template set<r>(i, xa o b_.template at<t>(i));             \
        i << c; }                                                       \
      else if (i.h[b].vn() == 1)                                        \
      { auto &c = f << (i.h << o9vec<r>{i.h[a].vn()});                  \
        let a_ = i.h[a];                                                \
        let c_ = i.h[c];                                                \
        let xb = i.h[b].template at<t>(0);                              \
        for (uN i = 0; i < a_.vn(); ++i)                                \
          c_.template set<r>(i, a_.template at<t>(i) o xb);             \
        i << c; }                                                       \
      else                                                              \
      { let k = std::min(i.h[a].vn(), i.h[b].vn());                     \
        auto &c = f << (i.h << o9vec<r>{k});                            \
        let a_ = i.h[a];                                                \
        let b_ = i.h[b];                                                \
        let c_ = i.h[c];                                                \
        for (uN i = 0; i < k; ++i)                                      \
          c_.template set<r>(                                           \
            i, a_.template at<t>(i) o b_.template at<t>(i));            \
        i << c; }});

#define a1fi(f, ...) f(i8,  __VA_ARGS__); f(i16, __VA_ARGS__); f(i32, __VA_ARGS__); f(i64, __VA_ARGS__);
#define a1fu(f, ...) f(u8,  __VA_ARGS__); f(u16, __VA_ARGS__); f(u32, __VA_ARGS__); f(u64, __VA_ARGS__);
#define a1ff(f, ...) f(f32, __VA_ARGS__); f(f64, __VA_ARGS__);
#define a1fc(f, ...) f(c32, __VA_ARGS__); f(c64, __VA_ARGS__);

#define a1cfi(f, g) f(g, i8);  f(g, i16); f(g, i32); f(g, i64);
#define a1cfu(f, g) f(g, u8);  f(g, u16); f(g, u32); f(g, u64);
#define a1cff(f, g) f(g, f32); f(g, f64);
#define a1cfc(f, g) f(g, c32); f(g, c64);

#define a1gena(f, g) f(g, +); f(g, -); f(g, *); f(g, /);
#define a1rela(f, g) f(g, ==); f(g, !=); f(g, <); f(g, >); f(g, <=); f(g, >=);
#define a1moda(f, g) f(g, %);
#define a1bita(f, g) f(g, &); f(g, |); f(g, ^); f(g, <<); f(g, >>);

  a1cfi(a1fi, a1sc);  a1cfi(a1fu, a1sc);  a1cfi(a1ff, a1sc);
  a1cfu(a1fi, a1sc);  a1cfu(a1fu, a1sc);  a1cfu(a1ff, a1sc);
  a1cff(a1fi, a1sc);  a1cff(a1fu, a1sc);  a1cff(a1ff, a1sc);
  a1cfc(a1fi, a1sc);  a1cfc(a1fu, a1sc);  a1cfc(a1ff, a1sc);

  a1cfi(a1fi, a1vc);  a1cfi(a1fu, a1vc);  a1cfi(a1ff, a1vc);
  a1cfu(a1fi, a1vc);  a1cfu(a1fu, a1vc);  a1cfu(a1ff, a1vc);
  a1cff(a1fi, a1vc);  a1cff(a1fu, a1vc);  a1cff(a1ff, a1vc);
  a1cfc(a1fi, a1vc);  a1cfc(a1fu, a1vc);  a1cfc(a1ff, a1vc);

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

#undef a1sc

#undef a1sop
#undef a1vop

#undef a1fi
#undef a1fu
#undef a1ff
#undef a1fc

#undef a1gena
#undef a1rela
#undef a1moda
#undef a1bita
}


#if τdebug
π0TG void π0abi1_debug(π0T(π0abi) &a)
{
  a .def(":gc",   I{ i.h.gc(); })
    .def(":gH",   I{ std::cout << i.h.gΘ << std::endl; })
    .def(":src",  I{ std::cout << i.p << std::endl; })
    .def(":int",  I{ std::cout << i << std::endl; })
    .def(":out",  I{ std::cout << i.h[i[0]] << std::endl; })
    .def(":data", I
         { for (uN j = 0; j < i.size(); ++j)
             std::cout << "[" << j << "]\t" << i.h[i[j]] << std::endl; });
}
#endif


#undef I


π0TG π0T(π0abi) const &π0abi1()
{
  static π0T(π0abi) a;
  static bool       i = false;

  if (i) return a;

  π0abi1_blocks(a);
  π0abi1_stack(a);
  π0abi1_frame(a);
  π0abi1_control(a);
  π0abi1_quote(a);
  π0abi1_number(a);

# if τdebug
  π0abi1_debug(a);
# endif

  return a;
}


}

#include "pi0-end.hh"


#endif
