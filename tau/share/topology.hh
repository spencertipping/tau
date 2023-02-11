#ifndef τshare_topology_h
#define τshare_topology_h

#include "../eta1o.hh"
#include "../gamma.hh"

#include "../begin.hh"

namespace τ
{


γ        γffn(St,   F<void(Sp<ψ>, ξi, ξo)>&&,   F<void(ψ&)>&& = [](ψ&){});
inline γ γffn(St n, F<void(       ξi, ξo)> &&f, F<void(ψ&)> &&xf = [](ψ&){}) { return γffn(n, [f=std::move(f)](Sp<ψ>, ξi i, ξo o) { f(i, o); }, std::move(xf)); }
inline γ γffn(St n, F<void(           ξo)> &&f, F<void(ψ&)> &&xf = [](ψ&){}) { return γffn(n, [f=std::move(f)](Sp<ψ>, ξi, ξo o) { f(o); }, std::move(xf)); }
inline γ γffn(St n, F<void(       ξi    )> &&f, F<void(ψ&)> &&xf = [](ψ&){}) { return γffn(n, [f=std::move(f)](Sp<ψ>, ξi i, ξo) { f(i); }, std::move(xf)); }

γ        γbfn(St,   F<void(Sp<ψ>, ξo, ξi)>&&,   F<void(ψ&)>&& = [](ψ&){});
inline γ γbfn(St n, F<void(       ξo, ξi)> &&f, F<void(ψ&)> &&xf = [](ψ&){}) { return γbfn(n, [f=std::move(f)](Sp<ψ>, ξo o, ξi i) { f(o, i); }, std::move(xf)); }
inline γ γbfn(St n, F<void(           ξi)> &&f, F<void(ψ&)> &&xf = [](ψ&){}) { return γbfn(n, [f=std::move(f)](Sp<ψ>, ξo, ξi i) { f(i); }, std::move(xf)); }
inline γ γbfn(St n, F<void(       ξo    )> &&f, F<void(ψ&)> &&xf = [](ψ&){}) { return γbfn(n, [f=std::move(f)](Sp<ψ>, ξo o, ξi) { f(o); }, std::move(xf)); }

γ γcat(V<γ>&&);


γ γcap(St = "",
       F<void(Sp<ψ>, ξo, ξi)>&&
       = [](Sp<ψ>, ξo o, ξi i) { for (let x : i) if (!(o << x)) break; });


// FIXME: this is emitting η₀ refs for some reason, even when used with
// string inputs
template<class T>
γ γonce(T const &k)
{ return γffn("i", [k=η1o(k)](Sp<ψ>, ξi i, ξo o) mutable
  { for (let x : i) if (!(o << x)) return;
    o << k; }); }


γ γeach(F<void(η)>&&, bool = false);

γ γmap (F<η0o(η)> &&f);
γ γτmap(F<η0o(η)> &&f);


}

#include "../end.hh"

#endif
