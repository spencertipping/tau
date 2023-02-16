#include <iostream>

#include "strings.hh"
#include "topology.hh"

#include "../begin.hh"

namespace τ
{


γ γsplit_chr(cs7 cs) { return γsplit_chr_bounded(cs, -1); }
γ γsplit_str(St  p)  { return γsplit_str_bounded(p,  -1); }


γ γsplit_chr_bounded(cs7 cs, uN limit)
{
  return γffn("γschr", [=](Sp<ψ>, ξi i, ξo o) mutable
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


γ γsplit_str_bounded(St p, uN limit)
{
  return γffn("γsstr", [=](Sp<ψ>, ξi i, ξo o) mutable
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


γ γtsv()
{
  return γsplit_chr("\n")
       | γmap([](η x) { return η1o(cs7{"\t"}.split(x.stv())); });
}


}

#include "../end.hh"
