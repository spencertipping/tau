
#include "sigma-Gamma-pieces.hh"
#include "begin.hh"

namespace σ
{

using namespace std::literals;
using namespace τ;


void πshared_cmp(πφ &p)
{
  p .def_spost(">",  [](πse<ηi> const &y, ηic &x) { return x > y.x; })
    .def_spost(">=", [](πse<ηi> const &y, ηic &x) { return x >= y.x; })
    .def_spost("<",  [](πse<ηi> const &y, ηic &x) { return x < y.x; })
    .def_spost("<=", [](πse<ηi> const &y, ηic &x) { return x <= y.x; })
    .def_spost("==", [](πse<ηi> const &y, ηic &x) { return (x <=> y.x) == PO::equivalent; })
    .def_spost("!=", [](πse<ηi> const &y, ηic &x) { return (x <=> y.x) != PO::equivalent; })
    ;
}


}

#include "end.hh"
