#include "loops.hh"
#include "topology.hh"
#include "../begin.hh"

namespace τ
{


Γ Γι(u64 u)
{
  return Γffn("ι", [u](ξi I, ξo o)
    { I.close();
      for (u64 i = 0; i < u && o << η1o(i); ++i); });
}


Γ Γϊ()
{
  return Γffn("ϊ", [](ξi i, ξo o)
    { for (let x : i)
        if (η0ts{η0t::uint_be, η0t::int_be}[x.t()])
        { let n = x.pu();
          for (u64 j = 0; j < n; ++j) if (!(o << η1o(j))) return;
          if (!(o << η1o(η1sig::τ))) return; }
        else
          if (!(o << x)) return; });
}


Γ Γtake(u64 n)
{
  return Γffn("↑", [=](ξi i, ξo o) mutable
    { for (let x : i) if (!n-- || !(o << x)) break; });
}


Γ Γdrop(u64 n)
{
  return Γffn("↓", [=](ξi i, ξo o) mutable
    { for (let x : i) if (!n--) break;
      for (let x : i) if (!(o << x)) break; });
}


Γ Σi()
{
  return Γffn("∑i", [](ξi i, ξo o)
    { u64 t = 0;
      for (let x : i)
        if (x.τ())
        { t = 0;
          if (!(o << η1o(η1sig::τ))) return; }
        else
          if (!(o << η1o(t += x.pi()))) return; });
}


Γ Σf()
{
  return Γffn("∑f", [](ξi i, ξo o)
    { f64 t = 0;
      for (let x : i)
        if (x.τ())
        { t = 0;
          if (!(o << η1o(η1sig::τ))) return; }
        else
          if (!(o << η1o(t += x.pf()))) return; });
}


}

#include "../end.hh"
