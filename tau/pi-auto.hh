#ifndef τπauto_h
#define τπauto_h

#include "ctypes.hh"
#include "eta-auto.hh"
#include "pi-fn.hh"
#include "pi-int.hh"
#include "pi-phi-markers.hh"
#include "begin.hh"

namespace τ
{


// See doc/pi-phi.md for an explanation of these classes
enum class πautoclass
{
  meta,
  constant,
  immediate,
  stack
};


Tt struct πautoclass_;
Tt concept πautometa  = πautoclass_<De<T>>::c == πautoclass::meta;
Tt concept πautoconst = πautoclass_<De<T>>::c == πautoclass::constant;
Tt concept πautoimmed = πautoclass_<De<T>>::c == πautoclass::immediate;
Tt concept πautostack = πautoclass_<De<T>>::c == πautoclass::stack;

template<>     struct πautoclass_<πi>      { sletc c = πautoclass::meta; };
template<uN N> struct πautoclass_<πhr_<N>> { sletc c = πautoclass::meta; };

template<is_πφ T> struct πautoclass_<T>     { sletc c = πautoclass::constant; };
Tt                struct πautoclass_<πP<T>> { sletc c = πautoclass::constant; };

// NOTE: πsa and friends can apply to both eager values (which are stack-compatible)
// and lazy values (which are encoded as πf<N>).
template<πautostack T> struct πautoclass_<πsa<T>> { sletc c = πautoclass::immediate; };
template<πautostack T> struct πautoclass_<πpa<T>> { sletc c = πautoclass::immediate; };
template<πautostack T> struct πautoclass_<πse<T>> { sletc c = πautoclass::immediate; };
template<πautostack T> struct πautoclass_<πpe<T>> { sletc c = πautoclass::immediate; };

template<iN N> struct πautoclass_<πsa<πf<N>>> { sletc c = πautoclass::constant; };
template<iN N> struct πautoclass_<πpa<πf<N>>> { sletc c = πautoclass::constant; };
template<iN N> struct πautoclass_<πse<πf<N>>> { sletc c = πautoclass::constant; };
template<iN N> struct πautoclass_<πpe<πf<N>>> { sletc c = πautoclass::constant; };

template<>               struct πautoclass_<πhr> { sletc c = πautoclass::stack; };
template<ηauto_decode T> struct πautoclass_<T>   { sletc c = πautoclass::stack; };


// Split all function args into separate lists, each of which occurs in the same
// order as the original
template<class... Xs> struct πautoclassify_;
template<> struct πautoclassify_<>
{
  using M = T<>;
  using C = T<>;
  using I = T<>;
  using S = T<>;
};

// NOTE: it's important to preserve ordering
template<πautometa X, class... Xs> struct πautoclassify_<X, Xs...>
{
  using M = typename Tcons_<X, typename πautoclassify_<Xs...>::M>::t;
  using C = typename πautoclassify_<Xs...>::C;
  using I = typename πautoclassify_<Xs...>::I;
  using S = typename πautoclassify_<Xs...>::S;
};

template<πautoconst X, class... Xs> struct πautoclassify_<X, Xs...>
{
  using M = typename πautoclassify_<Xs...>::M;
  using C = typename Tcons_<X, typename πautoclassify_<Xs...>::C>::t;
  using I = typename πautoclassify_<Xs...>::I;
  using S = typename πautoclassify_<Xs...>::S;
};

template<πautoimmed X, class... Xs> struct πautoclassify_<X, Xs...>
{
  using M = typename πautoclassify_<Xs...>::M;
  using C = typename πautoclassify_<Xs...>::C;
  using I = typename Tcons_<X, typename πautoclassify_<Xs...>::I>::t;
  using S = typename πautoclassify_<Xs...>::S;
};

template<πautostack X, class... Xs> struct πautoclassify_<X, Xs...>
{
  using M = typename πautoclassify_<Xs...>::M;
  using C = typename πautoclassify_<Xs...>::C;
  using I = typename πautoclassify_<Xs...>::I;
  using S = typename Tcons_<X, typename πautoclassify_<Xs...>::S>::t;
};


// TODO: dual-lambda conversion using the above classifiers


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
