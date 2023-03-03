#include <iostream>

#include "io.hh"
#include "topology.hh"
#include "../strings.hh"

#include "../begin.hh"

namespace τ
{


Γ Γostream(O &s, St prefix, bool tap)
{
  return Γ2("Γ»'", [=, &s](Sp<ψ>, ξi i, ξo o) mutable
    { for (let x : i)
      { s << prefix << x << std::endl;
        if (tap && !(o << x)) break; }});
}


Γ ΓZ(u8 l)
{
  return Γ2((Ss{} << "ΓZ" << Sc<uN>(l)).str(), [l](ξi i, ξo o)
    { for (let x : i)
        if (x.c() >= l) { if (!(o << x)) return; }
        else
        { η0o y(x.t());
          x.into(y.c(l).iptr(x.osize()));
          if (!(o << y)) return; }});
}


}

#include "../end.hh"
