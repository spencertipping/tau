#ifndef τshare_topology_h
#define τshare_topology_h

#include "../gamma.hh"

#include "../begin.hh"

namespace τ
{


Sp<γ> γffn(St, F<void(Sp<ψ>, ξi, ξo)>&&);
Sp<γ> γbfn(St, F<void(Sp<ψ>, ξo, ξi)>&&);

Sp<γ> γcat(V<Sp<γ>>&&);

Sp<γ> γonce(η0o const&);

Sp<γ> γeach(F<void(η0i)>&&, bool = false);
Sp<γ> γmap (F<η0o(η0i)>&&);
Sp<γ> γτmap(F<η0o(η0i)>&&);


}

#include "../end.hh"

#endif
