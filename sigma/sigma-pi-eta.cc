#include "sigma-Gamma-pieces.hh"
#include "begin.hh"

namespace σ
{

using namespace std::literals;
using namespace τ;


void πshared_η(πφ &p)
{
  p .def_sr("?", [](πi &i, bool c, πpc<π1> &y, φaL<':'>, πpc<π1> &z)
      { return (c ? y.x(i) : z.x(i)).pop(); })

    .def_pr("`", [](πi &i, πhrc&, πpc<π1> &y)
      { return y.x(i).pop(); })

    .def_sl(">\"", [](πsc<ηi> &x) { return St{Rc<chc*>(x.x.ldata()), x.x.lsize()}; })
    .def_sl(".|",  [](πsc<ηi> &x) { return i64(x.x.lsize()); })
    ;
}


}

#include "end.hh"
