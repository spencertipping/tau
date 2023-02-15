#include "pi.hh"
#include "share/topology.hh"

#include "begin.hh"

namespace τ
{


γ γπ(πfn const &f, St name, F<bool(πi&, πinsn const&)> each)
{
  if (name.empty()) name = (Ss{} << "π" << f).str();
  return γffn(name, [f, each](ξi i, ξo o) { πi i_{i, o}; f.run(i_, each); });
}


}

#include "end.hh"
