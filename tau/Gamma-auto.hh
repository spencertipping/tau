#ifndef τΓauto_h
#define τΓauto_h

#include "types.hh"
#include "Gamma.hh"
#include "Xi.hh"
#include "phi-auto.hh"
#include "begin.hh"

namespace τ
{


Tt         struct is_Γarg_    : std::false_type {};
template<> struct is_Γarg_<Ξ> : std::true_type {};

Tt concept is_Γarg = is_Γarg_<De<T>>::value;


template<class I, class O> struct Γauto__;
template<class... Is, class... Os>
struct Γauto__<T<Is...>, T<Os...>>
{
  Tt static auto f(T &&f)
    {
      return [f=mo(f)](Os... os) { return Γ{new Γf_("auto", [=, &f](Is... is) { return f(os..., is...); })}; };
    }
};


template<class... Xs>          struct Γauto_is_done_;
template<>                     struct Γauto_is_done_<> { sletc v = true; };
template<class X, class... Xs> struct Γauto_is_done_<X, Xs...>
{ sletc v = !is_Γarg<X> && Γauto_is_done_<Xs...>::v; };


template<uS I, class... Xs>
auto Γauto0_(F<Ξ(Xs...)> &&f)
{
  if constexpr (!is_Γarg<std::tuple_element_t<I, T<Xs...>>>)
                 return Γauto0_<I + 1>(mo(f));
  else return Γauto__<Tdrop<I, T<Xs...>>,
                      Ttake<I, T<Xs...>>>::f(mo(f));
}


template<class R, class... Xs>
auto Γauto1_(F<R(Xs...)> &&f)
{
  if constexpr (Γauto_is_done_<Xs...>::v) return f;
  else                                    return Γauto0_<0>(mo(f));
}


template<class T>
auto Γauto(T &&f)
{
  return Γauto1_(std::function(std::forward<T>(f)));
}


}

#include "end.hh"

#endif
