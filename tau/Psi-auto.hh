#ifndef τΨauto_h
#define τΨauto_h

#include "types.hh"
#include "Psi.hh"
#include "xiio.hh"
#include "phi-auto.hh"
#include "begin.hh"

namespace τ
{


// A bit of auto-magic for Ψ definitions, which involve inner lambdas.
// We can combine these into a single lambda by detecting arg types.
Tt         struct is_ψarg_       : std::false_type {};
template<> struct is_ψarg_<ψ>    : std::true_type {};
template<> struct is_ψarg_<ξi>   : std::true_type {};
template<> struct is_ψarg_<ξo>   : std::true_type {};
template<> struct is_ψarg_<Ψaux> : std::true_type {};

Tt concept is_ψarg = is_ψarg_<T>::value;


template<bool C, class I, class O> struct Ψauto__;
template<bool C, class... Is, class... Os>
struct Ψauto__<C, T<Is...>, T<Os...>>
{
  Tt static auto f(T &&f)
    {
      if constexpr (C) return [f=mo(f)](Os const&... os) mutable { return Ψc([=, &f](Is... is) mutable { f(os..., is...); }); };
      else             return [f=mo(f)](Os const&... os) mutable { return Ψ ([=, &f](Is... is) mutable { f(os..., is...); }); };
    }
};


// Curry a function of the form (parse_args..., psi_args...), returning
// an inner call to Ψ() wrapping the inner function.
template<iN I, bool C, class... Xs>
auto Ψauto_(F<void(Xs...)> &&f)
{
  if constexpr (!is_ψarg<std::tuple_element_t<I, T<Xs...>>>)
    return Ψauto_<I + 1, C>(mo(f));
  else
    return Ψauto__<C,
                   Tdrop<I, T<Xs...>>,
                   Ttake<I, T<Xs...>>>::f(mo(f));
}


template<bool C, class T>
auto Ψauto(T &&f)
{
  return Ψauto_<0, C>(std::function(f));
}


}

#include "end.hh"

#endif
