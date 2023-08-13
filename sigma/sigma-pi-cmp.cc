
#include "sigma-Gamma-pieces.hh"
#include "begin.hh"

namespace σ
{

using namespace std::literals;
using namespace τ;


void πshared_cmp(πφ &p)
{
  using case_pair = φaS<πse<π1>, πse<π1>>;
  using cases     = φaS<φaN<case_pair>,  // main cases
                        πse<π1>>;        // default


  p .def_spost(">",  [](πse<ηi> const &y, ηic &x) { return x > y.x; })
    .def_spost(">=", [](πse<ηi> const &y, ηic &x) { return x >= y.x; })
    .def_spost("<",  [](πse<ηi> const &y, ηic &x) { return x < y.x; })
    .def_spost("<=", [](πse<ηi> const &y, ηic &x) { return x <= y.x; })
    .def_spost("==", [](πse<ηi> const &y, ηic &x) { return (x <=> y.x) == PO::equivalent; })
    .def_spost("!=", [](πse<ηi> const &y, ηic &x) { return (x <=> y.x) != PO::equivalent; })

    .def_spost("{", [](πi &i, πP<φrbrace<cases>> const &cs, πhr x)
      { πhlv v{i.h()};
        v << x;
        let [c, d] = cs.x.x.x;
        for (let &kv : c.x)
        { let &[k, v] = kv.x;
          k.x(i);
          if (i[x] == i.ypop()) { v.x(i); return i.pop(); } }
        d.x(i);
        return i.pop(); })
    ;
}


}

#include "end.hh"
