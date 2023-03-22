#include "pi.hh"
#include "topology.hh"

#include "../begin.hh"

namespace τ
{


Γ Γπ(πfn const &f, St name, F<bool(πi&, πinsn const&)> each)
{
  if (name.empty()) name = (Ss{} << "π" << f).str();
  return Γffn(name, [f=Sp<πfn const>(new πfn{f}), each](ξi i, ξo o)
    { πi{f, i, o}.run(0, each); });
}


}

#include "../end.hh"
