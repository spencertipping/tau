#include "sigma-Gamma-pieces.hh"
#include "begin.hh"

namespace σ
{

using namespace std::literals;
using namespace τ;


void πshared_list(πφ &p)
{
  p .def_sl("@", [](πsc<ηi> &x) { return x.x.η().all(); })
    .def_sl("#", [](πsc<ηi> &x) { return i64(x.x.η().len()); })
    .def_sr("@[", [](πi &i, πhr x, πs<i64> s, φaL<','>, πs<i64> n, φaL<']'>)
      { return i.i(x, i[x].η().sub(s.x, n.x)); })

    .def_sl(">@", [](πi &i)
      { ηm m;
        for (let x : i.fi())
          if (x.is_τ()) break;
          else          m << x;
        return m; })

    .def_pr("|-", [](πi &i, ηic &x, πp<πhr> y)
      { for (let z : i[y.x]) i.fo().r() << x.all() << z.one().all(); return ηsig::τ; })
    .def_pr("-|", [](πi &i, ηic &x, πp<πhr> y)
      { for (let z : x) i.fo().r() << z.one().all() << i[y.x].all(); return ηsig::τ; })

    .def_pr(">|",  [](πi &i, πhr x) { return i.i(x, i[x].next()); })
    .def_pr(">>|", [](πi &i, πhr x) { return i.i(x, i[x].next().next()); })

    .def_pr("||-", [](πi &i, πhr x)
      { for (let y : i[x]) i.fo().r() << y.one().all(); return ηsig::τ; })
    .def_pr("@-",  [](πi &i, πhr x)
      { for (let y : i[x]) i.fo().r() << y.η().all(); return ηsig::τ; })
    ;
}


}

#include "end.hh"
