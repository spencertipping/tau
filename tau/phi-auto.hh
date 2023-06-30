#ifndef τφauto_h
#define τφauto_h

#include "phi.hh"
#include "phi-fn.hh"
#include "phi-str.hh"
#include "phi-ctor.hh"
#include "phi-markers.hh"

#include "begin.hh"

namespace τ
{


// φ auto: construct a parser that will produce values for a function's
// parameters, parsing each sequentially after the last one.
//
// Because parsers may rely on state, φauto() first accepts an object that
// provides the parser for a given type. This object should implement a
// p(T*) method that returns a parser for type T. (Note that the T* here
// is always null; it's just being used for type selection.)
//
// It's possible to define a parser for nullary functions; the result is
// a function that will consume no input, but will be called separately per
// parse.

template<class A, class... Xs, class T>
auto φauto_(A const &a, F<T(Xs...)> &&f)
  -> φ<decltype(f(std::declval<Xs>()...))>
{
  if constexpr (sizeof...(Xs) == 0)
    return φm(φR<int>(0), [f=mo(f)](int) -> T { return f(); });
  else
    return φm(φs("auto", a.p(null<De<Xs>>())...),
              [f=mo(f)](auto &&xs) -> T { return std::apply(f, xs); });
}

template<class A, class F>
auto φauto(A const &a, F const &f)
{
  if constexpr (!is_φ_<De<F>>::value) return φauto_(a, std::function(f));
  else                                return f;
}


// A bundle of automatic string parsers to mix into a grammar; see pi-phi.hh
// or Gamma-phi.hh for examples.
template<class A> struct φauto_str
{
  template<char... S> auto p(φaL<S...>*) const
    { return φm(φl(St{S...}), [](auto &&x) { return φaL<S...>{mo(x)}; }); }

  template<char... S> auto p(φaO<S...>*) const
    { return φm(φo(φl(St{S...})), [](auto &&x) { return φaCs<S...>{mo(x)}; }); }

  template<char... S, bool N, u32 L, u32 U> auto p(φaCs<N, L, U, S...>*) const
    { return φm(φcs(St{S...}.data(), N, L, U),
                [](auto &&x) { return φaCs<N, L, U, S...>{mo(x)}; }); }

  Txs auto p(φaA<Xs...>*) const
    { return φm(φa("φaA<...>", p(null<Xs>())...),
                [](auto &&x) { return φaA<Xs...>{mo(x)}; }); }
};


}

#include "end.hh"

#endif
