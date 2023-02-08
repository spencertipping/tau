#include <iostream>

#include "io.hh"
#include "topology.hh"
#include "../begin.hh"

namespace τ
{


Sp<γ> γostream(O &s, St prefix, bool tap)
{
  return γffn("γ»", [=, &s](Sp<ψ>, ξi i, ξo o) mutable
  { for (let x : i)
    { s << prefix << x << std::endl;
      if (tap && !(o << x)) break; }});
}



}

#include "../end.hh"
