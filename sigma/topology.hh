#ifndef τshare_topology_h
#define τshare_topology_h

#include "../eta1o.hh"
#include "../Gamma.hh"

#include "../begin.hh"

namespace τ
{


// TODO: this should be moved out to tau/


typedef F<P<ξi, ξo>(Sp<ψ>, Ξ&)> Γ2fn_;  // 2-ξ splice function
typedef F<void(Wp<ψ>, ξi, ξo)>  Γffn_;  // 2-ξ processor function
typedef F<void(ψ&)>             Γfin_;  // ψ finalizer


Γ2fn_ Γ2f();   // forward splice
Γ2fn_ Γ2b();   // backward splice
Γ2fn_ Γ2rc();  // right duplex cap

// TODO: splice functions for sub-duplexes

// TODO: higher-order splice functions
// TODO: entangled processor functions


// 2-ξ processor with given splice
Γ        Γ2(St,   Γffn_           &&,  Γ2fn_ &&   = Γ2f(), Γfin_ &&   = [](ψ&){});
inline Γ Γ2(St n, F<void(ξi, ξo)> &&f, Γ2fn_ &&f2 = Γ2f(), Γfin_ &&xf = [](ψ&){}) { return Γ2(n, [f=std::move(f)](Wp<ψ>, ξi i, ξo o) { f(i, o); }, std::move(f2), std::move(xf)); }
inline Γ Γ2(St n, F<void(    ξo)> &&f, Γ2fn_ &&f2 = Γ2f(), Γfin_ &&xf = [](ψ&){}) { return Γ2(n, [f=std::move(f)](Wp<ψ>, ξi i, ξo o) { f(o); },    std::move(f2), std::move(xf)); }
inline Γ Γ2(St n, F<void(ξi    )> &&f, Γ2fn_ &&f2 = Γ2f(), Γfin_ &&xf = [](ψ&){}) { return Γ2(n, [f=std::move(f)](Wp<ψ>, ξi i, ξo o) { f(i); },    std::move(f2), std::move(xf)); }


}

#include "../end.hh"

#endif
