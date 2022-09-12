#ifndef τξbasics_h
#define τξbasics_h


#include "begin.hh"

namespace τ::ξ
{


template<class I = i9, class O>
ϝ &io(Φ &f, F<O(I)> &&l)
{
  return *new ϝ(f, ϝ::ξι, [&, l = std::move(l)](ϝ &f)
    { for (let x : f) if (!(f << l(x))) break; });
}


ϝ &iota(Φ &f, uN n = -1)
{
  return *new ϝ(f, ϝ::ξι, [&, n](ϝ &f) { if (!(f < ι(n))) std::cout << "early exit" << std::endl; });
}


}

#include "end.hh"


#endif
