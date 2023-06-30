#ifndef τφctor_h
#define τφctor_h


#include "phi.hh"
#include "phi-fn.hh"
#include "phi-str.hh"

#include "begin.hh"

namespace τ
{


Txxs φ<X> φd(St n, Xs const&... xs) { return {new φd_<X>(n, xs...)}; }

Tt     φ<T>  φl(St s, T v)   { return {new φl_<T>(s, v)}; }
inline φ<St> φl(St s)        { return φl(s, s); }
Tt     φ<T>  φE(φ<T> p)      { return {new φE_<T>(p)}; }
Tt     φ<T>  φR(T x)         { return {new φR_<T>(x)}; }
Tt     φ<T>  φF()            { return {new φF_<T>}; }
Tt     φ<T>  φW(φ<T> p)      { return {new φW_<T>{p}}; }
Tt     φ<T>  φL(F<φ<T>()> f) { return {new φL_<T>{f}}; }

inline φ<St>    φcs (chc *s, bool n = false, uN min = 0, uN max = -1) { return {new φcs_(s, n, min, max)}; }
inline φ<St>    φucs(F<bool(u64)> f,         uN min = 0, uN max = -1) { return {new φucs_(f, min, max)}; }
inline φ<V<St>> φre (St re)                                           { return {new φre_(re)}; }


Tt φ<V<T>>  φn(φ<T> p, uN l = 0, uN u = -1) { return {new φn_<T>(p, l, u)}; }
Tt φ<Op<T>> φo(φ<T> p)                      { return {new φo_<T>(p)}; }
Tt φ<T>     φO(φ<T> p, T d)                 { return {new φO_<T>(p, d)}; }

Txs  φ<T<Xs...>> φs(St n, φ<Xs>... xs)        { return {new φs_(n, xs...)}; }
Txxs φ<V<X>>     φS(St n, Xs const&... p)     { return (new φS_<X>(n, p...)); }
Txs  φ<St>       φq(St n, φ<Xs>... xs)        { return {new φq_(φs(n, xs...))}; }

template<class... Xs, class R = De<decltype(std::get<0>(std::declval<T<Xs...>>()))>>
φ<R> φa(St n, φ<Xs> const&... xs) { return {new φa_<R>(n, xs...)}; }

Tt φ<T> φa0(St n) { return {new φa_<T>(n)}; }

template<class T, class F> φ<T> φf(φ<T> p, F f) { return {new φf_(p, f)}; }
template<class T, class F> auto φm(φ<T> p, F f)
  -> φ<De<decltype(f(std::declval<T>()))>>
{ return {new φm_<T, F>{p, f}}; }

template<class T, class F> auto φM(φ<T> p, F f)
  -> φ<De<typename decltype(f(std::declval<φc_>(), std::declval<φr_<T>>()))::R>>
{ return {new φM_<T, F>{p, f}}; }


Txs auto φ1(St n, φ<Xs>... xs)
{ return φm(φs(n, xs...), [](auto &&x) { return std::get<0>(x); }); }

Txs auto φ2(St n, φ<Xs>... xs)
{ return φm(φs(n, xs...), [](auto &&x) { return std::get<1>(x); }); }

Txs auto φ3(St n, φ<Xs>... xs)
{ return φm(φs(n, xs...), [](auto &&x) { return std::get<2>(x); }); }


φ<St> φCJK(uN min = 1, uN max = -1);


Tt                         φ<T>       operator| (φ<T> a, φ<T> b) { return φa("|",  a, b); }
template<class X, class Y> φ<T<X, Y>> operator& (φ<X> a, φ<Y> b) { return φs("&",  a, b); }
template<class T, class U> φ<T>       operator<<(φ<T> a, φ<U> b) { return φ1("<<", a, b); }
template<class T, class U> φ<U>       operator>>(φ<T> a, φ<U> b) { return φ1(">>", a, b); }
template<class T, class F> auto       operator* (φ<T> a, F b)    { return φm(a, b); }
template<class T, class F> auto       operator% (φ<T> a, F b)    { return φm(a, [b](auto &&x) { return std::apply(b, x); }); }


}

#include "end.hh"


#endif
