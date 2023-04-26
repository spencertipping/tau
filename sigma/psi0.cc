#include "auto.hh"
#include "psi0.hh"
#include "psi1.hh"
#include "begin.hh"

namespace σ
{

using namespace τ;


φ<Ψ0> φΨ0()
{
  static φ<Ψ0> r = φL<Ψ0>([]()
    { return φd<Ψ0>("Ψ0",
                    "F", φauto(ΓF)
#if τlinux
                    , ">F", φauto(ΓwF)
#endif
        );
    });
  return r;
}


Ψ0 ΓF()
{
  return [](ψ q, ξi i, Ψaux) { for (let x : i) (void) x; };
}


}

#include "end.hh"
