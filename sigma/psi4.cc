#include "psi4.hh"
#include "begin.hh"

namespace σ
{

using namespace τ;


φ<Ψ4> φΨ4()
{
  static φ<Ψ4> r = φL<Ψ4>([]()
    { return φd<Ψ4>("Ψ4",
                    "H", φauto(Γhttp)); });
  return r;
}


Ψ4 Γhttp()
{
  TODO("http");
  return Ψ4();
}


}

#include "end.hh"
