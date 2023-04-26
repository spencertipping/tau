#include "auto.hh"
#include "psi0.hh"
#include "psi1.hh"
#include "begin.hh"

namespace σ
{

using namespace τ;


φ<Ψ1> φΨ1()
{
  static φ<Ψ1> r = φL<Ψ1>([]()
    { return φd<Ψ1>("Ψ1",
                    "f", φauto(Γf),
                    "i", φauto(Γi),
                    "I", φauto(ΓI),
                    "n", φauto(Γn)
#if τlinux
                    , "<F", φauto(ΓrF)
                    , "T",  φauto(Γtcp)
#endif
        );
                    });
  return r;
}


Ψ1 Γf()
{
  return [](ψ q, ξo o, Ψaux)
    { τe &t = q.t();
      while (1) t.dt(now() + 1s); };
}

Ψ1 Γi(πv v)
{
  return [=](ψ q, ξo o, Ψaux) { o.r() << v; };
}

Ψ1 ΓI(πv v)
{
  return [=](ψ q, ξo o, Ψaux) { while (o) o.r() << v; };
}

Ψ1 Γn(i64 l)
{
  return [=](ψ q, ξo o, Ψaux)
    { for (i64 i = 0; i < l; ++i) o.r() << i; };
}


}

#include "end.hh"
