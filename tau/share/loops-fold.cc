#include "loops.hh"
#include "topology.hh"
#include "../begin.hh"

namespace τ
{


Γffn_ ξτ()
{
  return [](Wp<ψ>, ξi i, ξo o)
    { for (let x : i)
        if (!(o << x) || !(o << η1o(η1sig::τ))) break; };
}


Γffn_ ξτt(η0t t)
{
  return [t](Wp<ψ>, ξi i, ξo o)
    { η0o r(t);
      for (let x : i)
        if (x.τ())
        { if (!(o << r)) return;
          r.clear(); }
        else
          r << x; };
}


}

#include "../end.hh"
