#include "loops.hh"
#include "topology.hh"
#include "../begin.hh"

namespace τ
{


Γ Γτ()
{
  return Γffn("Γτ", [](Sp<ψ>, ξi i, ξo o) mutable
    { for (let x : i)
        if (!(o << x) || !(o << η1o(η1sig::τ))) break; });
}


Γ Γτt(η0t t)
{
  return Γffn("Γτ»()", [t](Sp<ψ>, ξi i, ξo o) mutable
    { η0o r(t);
      for (let x : i)
        if (x.τ())
        { if (!(o << r)) return;
          r.clear(); }
        else
          r << x; });
}


}

#include "../end.hh"
