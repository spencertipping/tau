#ifndef τξϝ_h
#define τξϝ_h


#include "begin.hh"

namespace τ::ξ
{


ϝ &splitter(Φ &f)
{
  return *new ϝ(f, ϝ::ξτ,
                [&](ϝ &f) { f.α() > f.δ(); },
                [&](ϝ &f) { f.ε() > f.α(); });
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
