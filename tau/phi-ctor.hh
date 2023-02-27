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

inline φ<St>    φcs (chc *s, bool n = false, uN min = 0, uN max = -1) { return φ<St>{new φcs_(s, n, min, max)}; }
inline φ<St>    φucs(F<bool(u64)> f,         uN min = 0, uN max = -1) { return φ<St>{new φucs_(f, min, max)}; }
inline φ<V<St>> φre (St re)                                           { return φ<V<St>>{new φre_(re)}; }

template<class T>              φ<T>       φE(φ<T> p)          { return φ<T>{new φE_<T>(p)}; }

template<class T, class... Xs> φ<T>       φa(Xs const&... xs) { return φ<T>{new φa_<T>(xs...)}; }

template<class T>              φ<V<T>>    φn(φ<T> p, uN l = 0, uN u = -1) { return φ<V<T>>{new φn_<T>(p, l, u)}; }

template<class T>              φ<St>      φq(φ<T> p)               { return φ<St>     {new φq_<T>(p)}; }

template<class T>              φ<Op<T>>   φo(φ<T> p)               { return φ<Op<T>>  {new φo_<T>(p)}; }
template<class T>              φ<T>       φO(φ<T> p, T d)          { return φ<T>      {new φO_<T>(p, d)}; }
template<class T, class U>     φ<P<T, U>> φs(φ<T> p, φ<U> q)       { return φ<P<T, U>>{new φs_<T, U>(p, q)}; }
template<class T, class U>     φ<U>       φm(φ<T> p, F<U(T)> f)    { return φ<U>      {new φm_<T, U>(p, f)}; }
template<class T>              φ<T>       φf(φ<T> p, F<bool(T)> f) { return φ<T>      {new φf_<T>(p, f)}; }
template<class T, class... Xs> φ<V<T>>    φS(Xs const&... p)       { return φ<V<T>>(new φS_<T>(p...)); }


template<class T> φ<T> φN(St n, φ<T> p) { return φ<T>{new φnamed_(n, p)}; }


// NOTE: char ranges from
// https://stackoverflow.com/questions/1366068/whats-the-complete-range-for-chinese-characters-in-unicode
inline φ<St> φCJK(uN min = 1, uN max = -1)
{
  return φucs([](u64 c)
    { return c >= 0x4e00  && c <= 0x9fcc
          || c >= 0x3400  && c <= 0x4db5
          || c >= 0x20000 && c <= 0x2a6df
          || c >= 0x2a700 && c <= 0x2b734
          || c >= 0x2b740 && c <= 0x2b81d; }, min, max);
}


// TODO: operator shorthands


template<class T>
Op<T> φparse(φ<T> p, Stc &x)
{
  return (*p)(φc_(x)).y;
}


}

#include "end.hh"


#endif
