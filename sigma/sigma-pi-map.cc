#include "sigma-Gamma-pieces.hh"
#include "begin.hh"

namespace σ
{

using namespace std::literals;
using namespace τ;


void πshared_map(πφ &p)
{
  p .def_sr(":", [](πi &i, πhrc &x, πPc<ηname> &n)
      { let r = i[x][n.x];
        return r.empty() ? i << ηsig::ω : i.i(x, r.one()); })

    .def_sl("@k", [](πi &i, πs<πhr> x)
      { πhlv hv{i.h()};
        hv << x.x;
        return i.r(i[x.x].lsize(), [&](auto &&r)
          { for (let &[k, v] : i[x.x].η().kvs()) r << k.one().all(); }); })

    .def_sl("@v", [](πi &i, πs<πhr> x)
      { πhlv hv{i.h()};
        hv << x.x;
        return i.r(i[x.x].lsize(), [&](auto &&r)
          { for (let &[k, v] : i[x.x].η().kvs()) r << v.one().all(); }); })

    .def_sr("@m>", [](πi &i, πhr x)
      { for (let &[k, v] : i[x].η().kvs()) i.fo().r() << k.one().all() << v.one().all();
        i.fo().r() << ηsig::τ;
        return πhr{}; })
    ;
}


}

#include "end.hh"
