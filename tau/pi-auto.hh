#ifndef τπauto_h
#define τπauto_h

#include "pi-fn.hh"
#include "begin.hh"

namespace τ
{


// Collects arguments from the interpreter stack and calls a function,
// returning its result.
//
// πhrs are passed through without transformation.
template<uS I, class R, class... Xs, class... Ys>
R πauto_(F<R(Xs...)> const &f, πi &i, Ys&&... ys)
{
  if constexpr (I == sizeof...(Xs)) return f(std::forward<Ys>(ys)...);
  else
  {
    using X = std::tuple_element_t<I, T<Xs...>>;
    if constexpr (Eq<X, πhr>)
      return πauto_<I + 1>(f, i, std::forward<Ys>(ys)..., i.pop());
    else
      return πauto_<I + 1>(f, i, std::forward<Ys>(ys)..., ηauto_<X>::v(i[i.pop()]));
  }
}


template<class R, class... Xs>
πf<1 - (int) sizeof...(Xs)> πvauto_(Stc &n, F<R(Xs...)> const &f)
{
  return {n, [=](πi &i)
    { if constexpr (Eq<R, πhr>) i.push(     πauto_<0>(f, i));
      else                      i.push(i << πauto_<0>(f, i)); }};
}

template<class F>
auto πvauto(Stc &n, F const &f)
{
  return πvauto_(n, std::function(f));
}


}

#include "end.hh"

#endif
