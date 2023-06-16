#ifndef τπauto_h
#define τπauto_h

#include "ctypes.hh"
#include "pi-fn.hh"
#include "pi-phi-markers.hh"
#include "begin.hh"

namespace τ
{


template<class... Xs> struct πPsplit_;

template<> struct πPsplit_<>
{
  using P = T<>;  // parse-time arguments
  using R = T<>;  // runtime arguments
};

template<class X, class... Xs> struct πPsplit_<X, Xs...>
{
  using P = Co<is_πP_<X>::value,
               typename Tcons_<T<X>, typename πPsplit_<Xs...>::P>::t,
               typename πPsplit_<Xs...>::R>;
  using R = Co<is_πP_<X>::value,
               typename πPsplit_<Xs...>::P,
               typename Tcons_<T<X>, typename πPsplit_<Xs...>::R>::t>;
};


template<class P, class R> struct πPsplit__;
template<class... Ps, class... Rs> struct πPsplit__<T<Ps...>, T<Rs...>>
{
  Tt static auto f(St n, F<T(Ps..., Rs...)> &&f)
    { return [n, f=mo(f)](Ps&&... ps)
      { return πvauto(n, [=, f=mo(f)](Rs&&... rs)
        { return f(ps..., rs...); }); }; }
};


// Split a function and prepare it to be used with φauto. This is called by
// πφ when defining grammar elements.
template<class T, class... Xs>
auto πPsplit(St n, F<T(Xs...)> &&f)
{
  return πPsplit__<typename πPsplit_<Xs...>::P,
                   typename πPsplit_<Xs...>::R>::f(n, f);
}


// Transform a single function argument from a stack entry to a C++ value.
Tt                struct πvauto1_          { sletc n = 1; static auto f(πi &i) { return ηauto_<T>::v(i[i.pop()]); } };
template<>        struct πvauto1_<πhr>     { sletc n = 1; static auto f(πi &i) { return i.pop(); } };
template<is_πv X> struct πvauto1_<X>       { sletc n = 1; static auto f(πi &i) { return X{πvauto1_<typename X::T>::f(i)}; } };
template<uN N>    struct πvauto1_<πhr_<N>> { sletc n = 0; static auto f(πi &i) { return πhr_<N>{}; } };
template<>        struct πvauto1_<πi&>     { sletc n = 0; static πi  &f(πi &i) { return i; } };


// Collects arguments from the interpreter stack and calls a function,
// returning its result.
//
// πhrs are passed through without transformation.
template<uS I, class R, class... Xs, class... Ys>
R πvauto__(F<R(Xs...)> const &f, πi &i, Ys&&... ys)
{
  if constexpr (I == sizeof...(Xs)) return f(std::forward<Ys>(ys)...);
  else
    return πvauto__<I + 1>(
      f, i, std::forward<Ys>(ys)...,
      πvauto1_<std::tuple_element_t<I, T<Xs...>>>::f(i));
}


// Count the number of stack pops that will happen to fill these arguments
// from the interpreter stack.
template<uS I, class... Xs> constexpr int πn_stackpops()
{
  if constexpr (I == sizeof...(Xs)) return 0;
  else return πvauto1_<std::tuple_element_t<I, T<Xs...>>>::n
         + πn_stackpops<I + 1, Xs...>();
}


// Convert a function's return value to a vertical series of η-castable
// things, each of which will be pushed as a separate stack entry.
Tt struct πvrauto_
{
  sletc n = 1;
  static void push(πi &i, T const &x)
    { if constexpr (Eq<T, πhr>) i.push(     x);
      else                      i.push(i << x); }
};

template<> struct πvrauto_<void>
{
  sletc n = 0;
};

template<class... Xs> struct πvrauto_<T<Xs...>>
{
  sletc n = sizeof...(Xs);
  static void push(πi &i, T<Xs...> const &x)
    { vpush<sizeof...(Xs) - 1>(i, x); }
  template<iN I> static void vpush(πi &i, T<Xs...> const &x)
    { πvrauto_<std::tuple_element_t<I, T<Xs...>>>::push(i, std::get<I>(x));
      if constexpr (I > 0) vpush<I - 1>(i, x); }
};


template<class R, class... Xs, class... Ys>
πf<πvrauto_<R>::n - πn_stackpops<0, Xs...>()>
πvauto_(Stc &n, F<R(Xs...)> const &f, Ys&&... ys)
{
  return {n, [=](πi &i)
    { if constexpr (Eq<R, void>) πvauto__<0>(f, i, ys...);
      else  πvrauto_<R>::push(i, πvauto__<0>(f, i, ys...)); }};
}

template<class F, class... Ys>
auto πvauto(Stc &n, F const &f, Ys&&... ys)
{
  return πvauto_(n, std::function(f), std::forward<Ys>(ys)...);
}


}

#include "end.hh"

#endif
