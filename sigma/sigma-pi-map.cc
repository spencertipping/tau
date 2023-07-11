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
    ;
}


}

#include "end.hh"
