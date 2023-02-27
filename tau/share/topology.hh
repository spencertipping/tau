#ifndef τshare_topology_h
#define τshare_topology_h

#include "../eta1o.hh"
#include "../Gamma.hh"

#include "../begin.hh"

namespace τ
{


Γ        Γffn(St,   F<void(Sp<ψ>, ξi, ξo)>&&,   F<void(ψ&)>&& = [](ψ&){});
inline Γ Γffn(St n, F<void(       ξi, ξo)> &&f, F<void(ψ&)> &&xf = [](ψ&){}) { return Γffn(n, [f=std::move(f)](Sp<ψ>, ξi i, ξo o) { f(i, o); }, std::move(xf)); }
inline Γ Γffn(St n, F<void(           ξo)> &&f, F<void(ψ&)> &&xf = [](ψ&){}) { return Γffn(n, [f=std::move(f)](Sp<ψ>, ξi, ξo o) { f(o); }, std::move(xf)); }
inline Γ Γffn(St n, F<void(       ξi    )> &&f, F<void(ψ&)> &&xf = [](ψ&){}) { return Γffn(n, [f=std::move(f)](Sp<ψ>, ξi i, ξo) { f(i); }, std::move(xf)); }

Γ        Γbfn(St,   F<void(Sp<ψ>, ξo, ξi)>&&,   F<void(ψ&)>&& = [](ψ&){});
inline Γ Γbfn(St n, F<void(       ξo, ξi)> &&f, F<void(ψ&)> &&xf = [](ψ&){}) { return Γbfn(n, [f=std::move(f)](Sp<ψ>, ξo o, ξi i) { f(o, i); }, std::move(xf)); }
inline Γ Γbfn(St n, F<void(           ξi)> &&f, F<void(ψ&)> &&xf = [](ψ&){}) { return Γbfn(n, [f=std::move(f)](Sp<ψ>, ξo, ξi i) { f(i); }, std::move(xf)); }
inline Γ Γbfn(St n, F<void(       ξo    )> &&f, F<void(ψ&)> &&xf = [](ψ&){}) { return Γbfn(n, [f=std::move(f)](Sp<ψ>, ξo o, ξi) { f(o); }, std::move(xf)); }

Γ Γcat(V<Γ>&&);


Γ Γcap(St = "",
       F<void(Sp<ψ>, ξi, ξo)>&&
       = [](Sp<ψ>, ξi i, ξo o) { for (let x : i) if (!(o << x)) break; });


template<class T>
Γ Γonce(T const &k)
{ return Γffn("i", [k=η1o(k)](Sp<ψ>, ξi i, ξo o)
  { for (let x : i) if (!(o << x)) return;
    o << k; }); }


Γ Γeach(F<void(η)>&&, bool = false);

Γ Γmap (F<η0o(η)> &&f);
Γ Γτmap(F<η0o(η)> &&f);


}

#include "../end.hh"

#endif
