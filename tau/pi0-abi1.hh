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


π0TG void π0abi1_blocks(π0T(π0abi) &a)
{
  a .def("[",  [](π0T(π0int) &i, uN s) { i.dpush(i.r.back()); i.r.back() += s; })
    .def("]",  [](π0T(π0int) &i, uN)   { i.r.pop_back(); })
    .def("[|", [](π0T(π0int) &i, uN n) { i.fpush(n); })
    .def("|]", [](π0T(π0int) &i, uN n) { i.fpop(); });
}


π0TG void π0abi1_stack(π0T(π0abi) &a)
{
  a .def(":",  [](π0T(π0int) &i, uN) { i << i[0]; })
    .def("::", [](π0T(π0int) &i, uN) { i << i[1]; i << i[1]; })
    .def("^",  [](π0T(π0int) &i, uN) { i << i[1]; })
    .def("^^", [](π0T(π0int) &i, uN) { i << i[2]; })
    .def("_",  [](π0T(π0int) &i, uN) { i.drop(1); })
    .def("%",  [](π0T(π0int) &i, uN)
      { let a = i[0];
        let b = i[1];
        i.drop(2);
        i << a; i << b; });
}


π0TG void π0abi1_frame(π0T(π0abi) &a)
{
  a .def("&:", [](π0T(π0int) &i, uN) { i.fpush(Sc<uN>(i.dpop())); })
    .def("&_", [](π0T(π0int) &i, uN) { i.fpop(); })
    .def("&@", [](π0T(π0int) &i, uN) { i << i.fi(Sc<uN>(i.dpop())); })
    .def("&=", [](π0T(π0int) &i, uN)
      { let v = Sc<uN>(i.dpop());
        i.fi(v) = i[0];
        i.drop(1); });
}


π0TG π0T(π0abi) const &π0abi1()
{
  static π0T(π0abi) a;
  static bool       i = false;

  if (i) return a;

  π0abi1_blocks(a);
  π0abi1_stack(a);
  π0abi1_frame(a);


  return a;
}


}

#include "pi0-end.hh"


#endif
