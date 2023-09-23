#include "sigma-Gamma-pieces.hh"
#include "begin.hh"

namespace σ
{

using namespace std::literals;
using namespace τ;


void πshared_map(πφ &p)
{
  p .def_spost(":", [](πi &i, πP<ηname> const &n, πhr const &x)
      { let r = i[x][n.x];
        return r.empty() ? i << ηsig::ω : i.i(x, r.one()); })

    .def_spre("@k", [](πi &i, πhr x)
      { πhlv hv{i.h()};
        hv << x;
        return i.r(i[x].lsize(), [&](auto &&r)
          { for (let &[k, v] : i[x].η().kvs()) r << k.one().all(); }); })

    .def_spre("@v", [](πi &i, πhr x)
      { πhlv hv{i.h()};
        hv << x;
        return i.r(i[x].lsize(), [&](auto &&r)
          { for (let &[k, v] : i[x].η().kvs()) r << v.one().all(); }); })
    ;
}


}

#include "end.hh"
