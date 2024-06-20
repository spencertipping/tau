#include "Omega.hh"
#include "../begin.hh"

namespace σ
{

using namespace τ;


void ΓΩ(Γφ &g)
{
  g.def_p1("@|O", [](Stc &path, ξo o) { Ωl l(path); for (let &x : l) o << x; });
}


}

#include "../end.hh"
