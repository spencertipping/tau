#ifndef τφctor_h
#define τφctor_h


#include "phi.hh"
#include "phi-fn.hh"
#include "phi-str.hh"

#include "begin.hh"

namespace τ
{


template<class T, class... Xs> φ<T> φd(Xs const&... xs) { return φ<T>{new φd_<T>(xs...)}; }
template<class T>              φ<T> φl(St s, T v)       { return φ<T>{new φl_<T>(s, v)}; }

inline φ<St>   φcs(chc *s, bool n = false, uN limit = -1) { return φ<St>  {new φcs_(s, n, limit)}; }
inline φ<bool> φE ()                                      { return φ<bool>{new φE_ ()}; }


template<class T, class... Xs> φ<T>   φa(Xs const&... xs) { return φ<T>{new φa_<T>(xs...)}; }

template<class T>          φ<V<T>>    φn(φ<T> p, uN l = 0, uN u = -1) { return φ<V<T>>{new φn_<T>(p, l, u)}; }

template<class T>          φ<T>       φo(φ<T> p)               { return φ<T>{new φo_<T>(p)}; }
template<class T, class U> φ<P<T, U>> φs(φ<T> p, φ<U> q)       { return φ<P<T, U>>{new φs_<T, U>(p, q)}; }
template<class T, class U> φ<U>       φm(φ<T> p, F<U(T)> f)    { return φ<U>{new φm_<T, U>(p, f)}; }
template<class T>          φ<T>       φf(φ<T> p, F<bool(T)> f) { return φ<T>{new φf_<T>(p, f)}; }


// TODO: operator shorthands


template<class T>
Op<T> φparse(φ<T> p, Stc &x)
{
  return (*p)(φc_(x)).y;
}


}

#include "end.hh"


#endif
