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
  // FIXME: all of these should just be "copy quoted value from
  // statics"
  a .def("i8",  I{ i.dpush(Sc<i8>(n)); })
    .def("i16", I{ i.dpush(Sc<i16>(n)); })
    .def("i32", I{ i.dpush(Sc<i32>(n)); })
    .def("i64", I{ i.dpush(Sc<i64>(n)); })

    .def("utf8", I{ i.dpush(i9{const_cast<ζp>(i.p.q.data() + n)}); })
    .def("sym",  I{ i.dpush(i9{const_cast<ζp>(i.p.q.data() + n)}); });
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

# if τdebug
  π0abi1_debug(a);
# endif

  return a;
}



}

#include "pi0-end.hh"


#endif
