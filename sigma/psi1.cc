#include "auto.hh"
#include "psi0.hh"
#include "psi1.hh"
#include "begin.hh"

namespace σ
{

using namespace τ;


φ<Ψ1> &φΨ1()
{
  static φ<Ψ1> r = φL<Ψ1>([]()
    { return φd<Ψ1>("Ψ1"
#if τlinux
                    , "<F", φauto(ΓrF)
                    , "T",  φauto(Γtcp)
#endif
        );
                    });
  return r;
}


}

#include "end.hh"
