#ifndef τξΘ_h
#define τξΘ_h


#include "begin.hh"

namespace τ::ξ
{


ϝ &θr(Φ &f, ΔΘ dt)
{
  return *new ϝ(f, "θr", ϝ::ξι, [&, dt](ϝ &f)
    { while (f.Θ(dt) << u9τ); });
}


ϝ &Δθ(Φ &f, ΔΘ dt)
{
  return *new ϝ(f, "Δθ", ϝ::ξι, [&, dt](ϝ &f)
    { for (let x : f) if (!(f.Θ(dt) << x)) break; });
}


ϝ &ΣθΔ(Φ &f)
{
  let s = new ΣΘΔ;
  return (new ϝ(f, "ΣθΔ", ϝ::ξι, [&, s](ϝ &f)
    { s->start();
      for (let x : f) { s->stop(); f.ε() << s->μ() / 1ns; s->start(); f << x; }
      s->stop(); }))->xf([s](ϝ&) { delete s; });
}


}

#include "end.hh"


#endif
