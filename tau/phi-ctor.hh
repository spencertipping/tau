#ifndef τφctor_h
#define τφctor_h


#include "phi.hh"
#include "phi-fn.hh"
#include "phi-str.hh"

#include "begin.hh"

namespace τ
{


template<class T, class... Xs> φ<T> φd(Xs const&... xs) { return {new φd_<T>(xs...)}; }
template<class T>              φ<T> φl(St s, T v)       { return {new φl_<T>(s, v)}; }

inline φ<St>    φcs (chc *s, bool n = false, uN min = 0, uN max = -1) { return {new φcs_(s, n, min, max)}; }
inline φ<St>    φucs(F<bool(u64)> f,         uN min = 0, uN max = -1) { return {new φucs_(f, min, max)}; }
inline φ<V<St>> φre (St re)                                           { return {new φre_(re)}; }

template<class T>              φ<T>       φE(φ<T> p)          { return {new φE_<T>(p)}; }

template<class T, class... Xs> φ<T>       φa(Xs const&... xs) { return {new φa_<T>(xs...)}; }

template<class T>              φ<V<T>>    φn(φ<T> p, uN l = 0, uN u = -1) { return {new φn_<T>(p, l, u)}; }

template<class T>              φ<St>       φq(φ<T> p)               { return {new φq_<T>(p)}; }
template<class T>              φ<Op<T>>    φo(φ<T> p)               { return {new φo_<T>(p)}; }
template<class T>              φ<T>        φO(φ<T> p, T d)          { return {new φO_<T>(p, d)}; }
template<class... Xs>          φ<T<Xs...>> φs(φ<Xs>... xs)          { return {new φs_(xs...)}; }
template<class T, class U>     φ<U>        φm(φ<T> p, F<U(T)> f)    { return {new φm_<T, U>(p, f)}; }
template<class T>              φ<T>        φf(φ<T> p, F<bool(T)> f) { return {new φf_<T>(p, f)}; }
template<class T, class... Xs> φ<V<T>>     φS(Xs const&... p)       { return (new φS_<T>(p...)); }


// NOTE: this is important to prevent infinite recursion for self-referential parsers
template<class T> φ<T> φN(St n, φ<T> p) { return {new φnamed_(n, p)}; }


φ<St> φCJK(uN min = 1, uN max = -1);


}

#include "end.hh"


#endif
