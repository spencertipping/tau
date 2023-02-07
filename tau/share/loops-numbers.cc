#include "loops.hh"
#include "topology.hh"
#include "../begin.hh"

namespace τ
{


Sp<γ> γι(u64 u)
{ return γffn("ι", [u](Sp<ψ>, ξi I, ξo o)
  { I.close();
    for (u64 i = 0; i < u && o << η1o(i); ++i); }); }


Sp<γ> γϊ()
{ return γffn("ϊ", [](Sp<ψ> q, ξi i, ξo o)
  { for (let x : i)
      if (η0ts{η0t::uint_be, η0t::int_be}[x.t()])
      { let n = x.pu();
        for (u64 j = 0; j < n; ++j) if (!(o << η1o(j))) goto done;
        if (!(o << η1o(η1sig::τ))) goto done; }
      else
        if (!(o << x)) goto done;
  done:; }); }


Sp<γ> γtake(u64 n)
{ return γffn("↑", [=](Sp<ψ> q, ξi i, ξo o) mutable
  { for (let x : i) if (!n-- || !(o << x)) break; }); }


Sp<γ> γdrop(u64 n)
{ return γffn("↓", [=](Sp<ψ> q, ξi i, ξo o) mutable
  { for (let x : i) if (!n--) break;
    for (let x : i) if (!(o << x)) break; }); }


}

#include "../end.hh"
