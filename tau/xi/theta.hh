#ifndef τξΘ_h
#define τξΘ_h


#include "begin.hh"


namespace τ::ξ
{


ϝ &Δθ(Φ &f, ΔΘ dt)
{
  return *new ϝ(f, ϝ::ξι, [&, dt](ϝ &f)
    { for (let x : f) if (!(f.Θ(dt) << x)) break; });
}


}


#include "end.hh"


#endif
