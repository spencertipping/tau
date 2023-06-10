#ifndef τπauto_h
#define τπauto_h

#include "pi-fn.hh"
#include "begin.hh"

namespace τ
{


// A marker to indicate that the π interpreter should reserve the specified
// number of bytes of memory in its heap before proceeding. This can force
// a GC to happen before any arguments are unpacked.
template<uN I> struct πhr_ { sletc s = I; };

template<class T> struct is_πhr_          : std::false_type {};
template<uN I>    struct is_πhr_<πhr_<I>> : std::true_type {};


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


// TODO: handle vertical → horizontal tuple returns

template<class R, class... Xs>
πf<1 - (int) sizeof...(Xs)> πvauto_(Stc &n, F<R(Xs...)> const &f)
{
  return {n, [=](πi &i)
    { if constexpr (Eq<R, πhr>) i.push(     πvauto__<0>(f, i));
      else                      i.push(i << πvauto__<0>(f, i)); }};
}

template<class F>
auto πvauto(Stc &n, F const &f)
{
  return πvauto_(n, std::function(f));
}


}

#include "end.hh"

#endif
