#include <iostream>

#include "strings.hh"
#include "topology.hh"

#include "../begin.hh"

namespace τ
{


Γ Γsplit_chr(cs7 cs) { return Γsplit_chr_bounded(cs, -1); }
Γ Γsplit_str(St  p)  { return Γsplit_str_bounded(p,  -1); }


Γ Γsplit_chr_bounded(cs7 cs, uN limit)
{
  return Γffn("Γschr", [=](Sp<ψ>, ξi i, ξo o) mutable
    { St b;
      for (let x : i)
        if (x.tsu() || x.tsb())
        { b.append(x.stv());
          if (b.size() > limit) { o << η1o(η1sig::ω); return; }
          let vs = cs.split(Stv{b});
          for (uN j = 0; j + 1 < vs.size(); ++j)
            if (!(o << η1o(vs[j]))) return;
          if (vs.size() > 1) b = vs.back(); }
        else if (x.τ())
        { if (!b.empty() && !(o << η1o(b))) return;
          b.clear();
          if (!(o << η1o(η1sig::τ))) return; }
      if (!b.empty()) o << η1o(b); });
}


Γ Γsplit_str_bounded(St p, uN limit)
{
  return Γffn("Γsstr", [=](Sp<ψ>, ξi i, ξo o) mutable
    { St b;
      for (let x : i)
        if (x.tsu() || x.tsb())
        { b.append(x.stv());
          if (b.size() > limit) { o << η1o(η1sig::ω); return; }
          let vs = ssplit(p, Stv{b});
          for (uN j = 0; j + 1 < vs.size(); ++j)
            if (!(o << η1o(vs[j]))) return;
          if (vs.size() > 1) b = vs.back(); }
        else if (x.τ())
        { if (!b.empty() && !(o << η1o(b))) return;
          b.clear();
          if (!(o << η1o(η1sig::τ))) return; }
      if (!b.empty()) o << η1o(b); });
}


Γ Γtsv()
{
  return Γsplit_chr("\n")
       | Γmap([](η x) { return η1o(cs7{"\t"}.split(x.stv())); });
}


}

#include "../end.hh"
