#include <iostream>

#include "strings.hh"
#include "topology.hh"

#include "../begin.hh"

namespace τ
{


γ γsplit_chr(cs7 cs)
{
  return γffn("γschr", [cs](Sp<ψ>, ξi i, ξo o) mutable
    { St b;
      for (let x : i)
        if (x.tsu() || x.tsb())
        { b.append(x.stv());
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


γ γsplit_str(St p)
{
  return γffn("γsstr", [p](Sp<ψ>, ξi i, ξo o) mutable
    { St b;
      for (let x : i)
        if (x.tsu() || x.tsb())
        { b.append(x.stv());
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
