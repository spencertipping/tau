
#include "sigma-Gamma-pieces.hh"
#include "begin.hh"

namespace σ
{

using namespace std::literals;
using namespace τ;


void πshared_cmp(πφ &p)
{
  using case_pair = φaS<πs<π1>, πs<π1>>;
  using cases     = φaS<φaN<case_pair>,  // main cases
                        πs<π1>>;         // default


  p .def_sr(">",  [](ηic &x, πs<ηi> const &y) { return x > y.x; })
    .def_sr(">=", [](ηic &x, πs<ηi> const &y) { return x >= y.x; })
    .def_sr("<",  [](ηic &x, πs<ηi> const &y) { return x < y.x; })
    .def_sr("<=", [](ηic &x, πs<ηi> const &y) { return x <= y.x; })
    .def_sr("==", [](ηic &x, πs<ηi> const &y) { return (x <=> y.x) == PO::equivalent; })
    .def_sr("!=", [](ηic &x, πs<ηi> const &y) { return (x <=> y.x) != PO::equivalent; })

    .def_sr("{", [](πi &i, πhr x, πP<φrbrace<cases>> const &cs)
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
