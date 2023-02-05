#ifndef τshare_topology_h
#define τshare_topology_h

#include "../gamma.hh"

#include "../begin.hh"

namespace τ
{


Sp<γ>        γffn(St,   F<void(Sp<ψ>, ξi, ξo)>&&,   F<void(ψ&)>&& = [](ψ&){});
inline Sp<γ> γffn(St n, F<void(       ξi, ξo)> &&f, F<void(ψ&)> &&xf) { return γffn(n, [f=std::move(f)](Sp<ψ>, ξi i, ξo o) { f(i, o); }, std::move(xf)); }
inline Sp<γ> γffn(St n, F<void(           ξo)> &&f, F<void(ψ&)> &&xf) { return γffn(n, [f=std::move(f)](Sp<ψ>, ξi, ξo o) { f(o); }, std::move(xf)); }
inline Sp<γ> γffn(St n, F<void(       ξi    )> &&f, F<void(ψ&)> &&xf) { return γffn(n, [f=std::move(f)](Sp<ψ>, ξi i, ξo) { f(i); }, std::move(xf)); }

Sp<γ> γbfn(St, F<void(Sp<ψ>, ξo, ξi)>&&, F<void(ψ&)>&& = [](ψ&){});
inline Sp<γ> γbfn(St n, F<void(       ξo, ξi)> &&f, F<void(ψ&)> &&xf) { return γbfn(n, [f=std::move(f)](Sp<ψ>, ξo o, ξi i) { f(o, i); }, std::move(xf)); }
inline Sp<γ> γbfn(St n, F<void(           ξi)> &&f, F<void(ψ&)> &&xf) { return γbfn(n, [f=std::move(f)](Sp<ψ>, ξo, ξi i) { f(i); }, std::move(xf)); }
inline Sp<γ> γbfn(St n, F<void(       ξo    )> &&f, F<void(ψ&)> &&xf) { return γbfn(n, [f=std::move(f)](Sp<ψ>, ξo o, ξi) { f(o); }, std::move(xf)); }

Sp<γ> γcat(V<Sp<γ>>&&);


template<class T>
Sp<γ> γonce(T const &k)
{ return γffn("i", [k=η0o(k)](Sp<ψ>, ξi i, ξo o) mutable
    { for (let x : i) if (!(o << x)) return;
      o << k; }); }


Sp<γ> γeach(F<void(η0i)>&&, bool = false);

Sp<γ> γmap (F<η0o(η0i)> &&f);
Sp<γ> γτmap(F<η0o(η0i)> &&f);


}

#include "../end.hh"

#endif
