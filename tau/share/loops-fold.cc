#include "loops.hh"
#include "topology.hh"
#include "../begin.hh"

namespace τ
{


Sp<γ> γτt(η0t t)
{
  return γffn("γτ()", [t](Sp<ψ>, ξi i, ξo o) mutable
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
