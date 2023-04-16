#ifndef τφctor_h
#define τφctor_h


#include "phi.hh"
#include "phi-fn.hh"
#include "phi-str.hh"

#include "begin.hh"

namespace τ
{


template<class T, class... Xs> φ<T>  φd(St n, Xs const&... xs) { return {new φd_<T>(n, xs...)}; }

template<class T>              φ<T>  φl(St s, T v) { return {new φl_<T>(s, v)}; }
inline                         φ<St> φl(St s)      { return φl(s, s); }
template<class T>              φ<T>  φE(φ<T> p)    { return {new φE_<T>(p)}; }
template<class T>              φ<T>  φR(T x)       { return {new φR_<T>(x)}; }
template<class T>              φ<T>  φF()          { return {new φF_<T>}; }
template<class T>              φ<T>  φW(φ<T> p)    { return {new φW_<T>{p}}; }

inline φ<St>    φcs (chc *s, bool n = false, uN min = 0, uN max = -1) { return {new φcs_(s, n, min, max)}; }
inline φ<St>    φucs(F<bool(u64)> f,         uN min = 0, uN max = -1) { return {new φucs_(f, min, max)}; }
inline φ<V<St>> φre (St re)                                           { return {new φre_(re)}; }


template<class T>              φ<V<T>>     φn(φ<T> p, uN l = 0, uN u = -1) { return {new φn_<T>(p, l, u)}; }

template<class T>              φ<Op<T>>    φo(φ<T> p)                   { return {new φo_<T>(p)}; }
template<class T>              φ<T>        φO(φ<T> p, T d)              { return {new φO_<T>(p, d)}; }
template<class... Xs>          φ<T<Xs...>> φs(St n, φ<Xs>... xs)        { return {new φs_(n, xs...)}; }
template<class T, class... Xs> φ<V<T>>     φS(St n, Xs const&... p)     { return (new φS_<T>(n, p...)); }
template<class... Xs>          φ<St>       φq(St n, φ<Xs>... xs)        { return {new φq_(φs(n, xs...))}; }

template<class... Xs, class R = De<decltype(std::get<0>(std::declval<T<Xs...>>()))>>
φ<R> φa(St n, φ<Xs> const&... xs) { return {new φa_<R>(n, xs...)}; }

template<class T> φ<T> φa0(St n) { return {new φa_<T>(n)}; }

template<class T, class F> φ<T> φf(φ<T> p, F f) { return {new φf_(p, f)}; }
template<class T, class F> auto φm(φ<T> p, F f)
  -> φ<De<decltype(f(std::declval<T>()))>>
{ return {new φm_<T, F>{p, f}}; }


template<class... Xs> auto φ1(St n, φ<Xs>... xs)
{ return φm(φs(n, xs...), [](auto &&x) { return std::get<0>(x); }); }

template<class... Xs> auto φ2(St n, φ<Xs>... xs)
{ return φm(φs(n, xs...), [](auto &&x) { return std::get<1>(x); }); }

template<class... Xs> auto φ3(St n, φ<Xs>... xs)
{ return φm(φs(n, xs...), [](auto &&x) { return std::get<2>(x); }); }


φ<St> φCJK(uN min = 1, uN max = -1);


}

#include "end.hh"


#endif
