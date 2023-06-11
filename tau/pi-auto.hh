#ifndef τπauto_h
#define τπauto_h

#include "ctypes.hh"
#include "pi-fn.hh"
#include "begin.hh"

namespace τ
{


// A marker to indicate that the π interpreter should reserve the specified
// number of bytes of memory in its heap before proceeding. This can force
// a GC to happen before any arguments are unpacked.
template<uN I> struct πhr_ { sletc s = I; };

Tt             struct is_πhr_          : std::false_type {};
template<uN I> struct is_πhr_<πhr_<I>> : std::true_type {};

Tt                    struct is_tuple_           : std::false_type {};
template<class... Xs> struct is_tuple_<T<Xs...>> : std::true_type {};


// Collects arguments from the interpreter stack and calls a function,
// returning its result.
//
// πhrs are passed through without transformation.
template<uS I, class R, class... Xs, class... Ys>
R πvauto__(F<R(Xs...)> const &f, πi &i, Ys&&... ys)
{
  if constexpr (I == sizeof...(Xs)) return f(std::forward<Ys>(ys)...);
  else
  {
    using X = std::tuple_element_t<I, T<Xs...>>;
    if constexpr (Eq<X, πi&>)
      return πvauto__<I + 1>(f, i, std::forward<Ys>(ys)..., i);
    else if constexpr (is_πhr_<X>::value)
    {
      i.h().reserve(X::s);
      return πvauto__<I + 1>(f, i, std::forward<Ys>(ys)..., {});
    }
    else if constexpr (Eq<X, πhr>)
      return πvauto__<I + 1>(f, i, std::forward<Ys>(ys)..., i.pop());
    else
      return πvauto__<I + 1>(f, i, std::forward<Ys>(ys)..., ηauto_<X>::v(i[i.pop()]));
  }
}


// Count the number of stack pops that will happen to fill these arguments
// from the interpreter stack.
template<uS I, class... Xs> constexpr int πn_stackpops()
{
  if constexpr (I == sizeof...(Xs)) return 0;
  else
  {
    using X = std::tuple_element_t<I, T<Xs...>>;
    if constexpr (is_πhr_<X>::value || Eq<X, πi&>) return     πn_stackpops<I + 1, Xs...>();
    else                                           return 1 + πn_stackpops<I + 1, Xs...>();
  }
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


template<class R, class... Xs>
πf<πvrauto_<R>::n - πn_stackpops<0, Xs...>()>
πvauto_(Stc &n, F<R(Xs...)> const &f)
{
  return {n, [=](πi &i)
    { if constexpr (Eq<R, void>) πvauto__<0>(f, i);
      else  πvrauto_<R>::push(i, πvauto__<0>(f, i)); }};
}

template<class F>
auto πvauto(Stc &n, F const &f)
{
  return πvauto_(n, std::function(f));
}


}

#include "end.hh"

#endif
