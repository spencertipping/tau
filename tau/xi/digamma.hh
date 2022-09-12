#ifndef τξϝ_h
#define τξϝ_h


#include "begin.hh"

namespace τ::ξ
{


ϝ &splitter(Φ &f)
{
  return *new ϝ(f, ϝ::ξτ,
                [&](ϝ &f) { f.α() > f.ε(); },
                [&](ϝ &f) { f.δ() > f.α(); });
}


ϝ &tee(Φ &f)
{
  let ps = new S<φi>;
  return (new ϝ(f, ϝ::ξβ,
                [&, ps](ϝ &f) { for (let x : f.β()) ps->emplace(x); },
                [&, ps](ϝ &f, γ &g)
                  { V<φi> c;
                    for (let x : f.α())
                    { for (let p : *ps)
                        if (!(f[p] <<= x)) c.push_back(p);
                      for (let p : c) { g.φx(p); ps->erase(p); }
                      c.clear(); }}))
    ->xf([ps](ϝ&) { delete ps; });
}


ϝ &broadcast(Φ &f)
{
  return *new ϝ(f, ϝ::ξγ, [&](ϝ &f)
    { S<φi> ps;
      for (let c : f)
      { let i = Sc<φi>(c);  // important: i9s are mutable
        f.ε() <<= o9t(i, true);
        f.λc([&, i](ϝ &f) {
          ps.emplace(i);
          for (let x : f[i]) for (let q : ps) f[q] <<= x;
          ps.erase(i);
          f.ε() <<= o9t(i, false);
        }); }});
}


}

#include "end.hh"


#endif
