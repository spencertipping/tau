#include "sigma-Gamma-pieces.hh"
#include "begin.hh"

namespace σ
{

using namespace std::literals;
using namespace τ;


void πshared_η(πφ &p)
{
  p .def_spost("?", [](πi &i, πpe<π1> const &y, φaL<':'>, πpe<π1> const &z, bool c)
      { return (c ? y.x(i) : z.x(i)).pop(); })

    .def_ppost("`", [](πi &i, πpe<π1> const &y, πhr const&)
      { return y.x(i).pop(); })

    .def_spre(">s", [](ηic &x) { return (Ss{} << x << "\n").str(); })
    ;
}


}

#include "end.hh"
