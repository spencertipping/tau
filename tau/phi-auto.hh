#ifndef τφauto_h
#define τφauto_h

#include "phi.hh"
#include "phi-fn.hh"
#include "phi-str.hh"
#include "phi-ctor.hh"

#include "begin.hh"

namespace τ
{


Tt struct φauto_;


template<class... Xs, class T>
auto φauto(F<T(Xs...)> const &f) -> φ<decltype(f(std::declval<Xs>()...))>
{
  if constexpr (sizeof...(Xs) == 0)
                 return φm(φR<int>(0), [f](int) -> T { return f(); });
  else
    return φm(φs("auto", φauto_<De<Xs>>::p()...),
              [f](auto xs) -> T { return std::apply(f, xs); });
}

template<class... Xs, class T>
auto φauto(T(*f)(Xs...))
{
  return φauto(std::function(f));
}


}

#include "end.hh"

#endif
