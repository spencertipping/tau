#include "sigma-Gamma-pieces.hh"
#include "begin.hh"

namespace σ
{

using namespace std::literals;
using namespace τ;


void πshared_list(πφ &p)
{
  p .def_spre("@", [](ηic &x) { return x.η().all(); })
    .def_spre("#", [](ηic &x) { return i64(x.η().len()); })

    .def_ppost("@<<", [](πi &i, πpe<πhr> y, ηic &x)
      { for (let z : i[y.x]) i.fo().r() << x.all() << z.one().all(); return ηsig::τ; })
    ;
}


}

#include "end.hh"
