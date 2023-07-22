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
    .def_sa([](φaL<'>', '@'>) { return π1{">@", [](πi &i)
      { ηm m;
        for (let x : i.fi())
          if (x.is_τ()) break;
          else          m << x;
        i.push(i << m); }}; })

    .def_ppost("|-", [](πi &i, πpe<πhr> y, ηic &x)
      { for (let z : i[y.x]) i.fo().r() << x.all() << z.one().all(); return ηsig::τ; })
    .def_ppost("-|", [](πi &i, πpe<πhr> y, ηic &x)
      { for (let z : x) i.fo().r() << z.one().all() << i[y.x].all(); return ηsig::τ; })

    .def_ppre("||-", [](πi &i, πhr x)
      { for (let y : i[x]) i.fo().r() << y.one().all(); return ηsig::τ; })
    .def_ppre("@-",  [](πi &i, πhr x)
      { for (let y : i[x]) i.fo().r() << y.η().all(); return ηsig::τ; })
    ;
}


}

#include "end.hh"
