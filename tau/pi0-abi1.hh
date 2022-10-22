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


#define I [](π0T(π0int) &i, uN n)

π0TG void π0abi1_blocks(π0T(π0abi) &a)
{
  a .def("[",  I{ i.dpush(i.r.back()); i.r.back() += n; })
    .def("]",  I{ i.r.pop_back(); })
    .def("[|", I{ i.fpush(n); })
    .def("|]", I{ i.fpop(); });
}


π0TG void π0abi1_stack(π0T(π0abi) &a)
{
  a .def(":",  I{ i << i[0]; })
    .def("::", I{ i << i[1]; i << i[1]; })
    .def("^",  I{ i << i[1]; })
    .def("^^", I{ i << i[2]; })
    .def("_",  I{ i.drop(1); })
    .def("%",  I
         { let a = i[0];
           let b = i[1];
           i.drop(2);
           i << a;
           i << b; });
}


π0TG void π0abi1_frame(π0T(π0abi) &a)
{
  a .def("&:", I{ i.fpush(Sc<uN>(i.dpop())); })
    .def("&_", I{ i.fpop(); })
    .def("&@", I{ i << i.fi(Sc<uN>(i.dpop())); })
    .def("&=", I
         { let v = Sc<uN>(i.dpop());
           i.fi(v) = i[0];
           i.drop(1); });
}


π0TG void π0abi1_control(π0T(π0abi) &a)
{
  // FIXME: define a portable type alias for bytecode offsets

  a .def(".",  I{ i.r.push_back(Sc<uN>(i.dpop())); })
    .def("?.", I
         { let e = Sc<uN>(i.dpop());
           let t = Sc<uN>(i.dpop());
           let c = Sc<bool>(i.dpop());
           i.run(c ? t : e); })
    .def("?!", I
         { let c = Sc<uN>(i.dpop());
           let b = Sc<uN>(i.dpop());
         loop:
           i.run(c);
           if (Sc<bool>(i.dpop()))
           { i.run(b);
             goto loop; }})
    .def(".^", I{ i.r.pop_back(); });
}


π0TG void π0abi1_numbers(π0T(π0abi) &a)
{
  a .def("i8",  I{ i.dpush(Sc<i8>(n)); })
    .def("i16", I{ i.dpush(Sc<i16>(n)); })
    .def("i32", I{ i.dpush(Sc<i32>(n)); })
    .def("i64", I{ i.dpush(Sc<i64>(n)); });
}


#if τdebug
π0TG void π0abi1_debug(π0T(π0abi) &a)
{
  a .def(":out",  I{ std::cout << i.h[i[0]] << std::endl; })
    .def("::out", I
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
  π0abi1_numbers(a);

# if τdebug
  π0abi1_debug(a);
# endif

  return a;
}



}

#include "pi0-end.hh"


#endif
