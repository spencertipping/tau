#include "loops.hh"
#include "topology.hh"
#include "../begin.hh"

namespace τ
{


Sp<γ> γι(uN u)
{ return γffn("ι", [u](Sp<ψ>, ξi I, ξo o)
  { I.close();
    for (u64 i = 0; i < u && o << η0o(i); ++i);
    o.close(); }); }


Sp<γ> γϊ()
{ return γffn("ϊ", [](Sp<ψ> q, ξi i, ξo o)
  { for (let x : i)
      if (η0ts{η0t::uint_be, η0t::int_be}[x.type()])
      { let n = Sc<i64>(η1pi{x});
        for (i64 j = 0; j < n; ++j) if (!(o << η0o{j})) goto done;
        o << η0o(η0sig::τ); }
      else
        if (!(o << x)) goto done;
  done:
    o.close(); }); }


Sp<γ> γtake(u64 n)
{ return γffn("↑", [=](Sp<ψ> q, ξi i, ξo o) mutable
  { for (let x : i) if (!n-- || !(o << x)) break;
    o.close(); }); }


Sp<γ> γdrop(u64 n)
{ return γffn("↓", [=](Sp<ψ> q, ξi i, ξo o) mutable
  { for (let x : i) if (!n--) break;
    for (let x : i) if (!(o << x)) break;
    o.close(); }); }


}

#include "../end.hh"
