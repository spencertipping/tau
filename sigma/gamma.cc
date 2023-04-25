#include "gamma.hh"
#include "begin.hh"

namespace σ
{

using namespace τ;


φ<Γ> φΓa()
{
  return φF<Γ>();
}


φ<Γ> φΓ()
{
  return Γφ(φΓa(), φΨ0(), φΨ1(), φΨ2(), φF<Ψ4>());
}


}

#include "end.hh"
