#ifndef τφauto_h
#define τφauto_h

#include "phi.hh"
#include "phi-fn.hh"
#include "phi-str.hh"
#include "phi-ctor.hh"

#include "begin.hh"

namespace τ
{


// φ auto: construct a parser that will produce values for a function's
// parameters, parsing each sequentially after the last one.
//
// Because parsers may rely on state, φauto() first accepts an object that
// provides the parser for a given type. This object should implement a
// p(T&&) method that returns a parser for type T.
//
// It's possible to define a parser for nullary functions; the result is
// a function that will consume no input, but will be called separately per
// parse.

template<class A, class... Xs, class T>
auto φauto(A const &a, F<T(Xs...)> const &f) -> φ<decltype(f(std::declval<Xs>()...))>
{
  if constexpr (sizeof...(Xs) == 0)
    return φm(φR<int>(0), [f](int) -> T { return f(); });
  else
    return φm(φs("auto", a.p(std::declval<De<Xs>>())...),
              [f](auto &&xs) -> T { return std::apply(f, xs); });
}

template<class A, class... Xs, class T>
auto φauto(A const &a, T(*f)(Xs...))
{
  return φauto(a, std::function(f));
}


}

#include "end.hh"

#endif
