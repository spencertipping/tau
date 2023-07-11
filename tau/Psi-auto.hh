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
Tt struct is_ψarg_       : std::false_type {};
Tn struct is_ψarg_<ψ>    : std::true_type {};
Tn struct is_ψarg_<ξi>   : std::true_type {};
Tn struct is_ψarg_<ξo>   : std::true_type {};
Tn struct is_ψarg_<Ψaux> : std::true_type {};

Tt concept is_ψarg = is_ψarg_<T>::value;


template<bool C, class I, class O> struct Ψauto__;
template<bool C, class... Is, class... Os>
struct Ψauto__<C, T<Is...>, T<Os...>>
{
  Tt static auto f(T &&f)
    {
      if constexpr (C) return [f=mo(f)](Os... os) { return Ψc([=, &f](Is&&... is) { f(os..., std::forward<Is>(is)...); }); };
      else             return [f=mo(f)](Os... os) { return Ψ ([=, &f](Is&&... is) { f(os..., std::forward<Is>(is)...); }); };
    }
};


// Detect already-auto-transformed functions so we can return them directly.
// In this case, "done" means there are no ψ-specific args.
Txs  struct Ψauto_is_done_;
Tn   struct Ψauto_is_done_<>         { sletc v = true; };
Txxs struct Ψauto_is_done_<X, Xs...> { sletc v = !is_ψarg<X> && Ψauto_is_done_<Xs...>::v; };


// Curry a function of the form (parse_args..., psi_args...), returning
// an inner call to Ψ() wrapping the inner function.
template<uS I, bool C, class... Xs>
auto Ψauto0_(F<void(Xs...)> &&f)
{
  if constexpr (!is_ψarg<std::tuple_element_t<I, T<Xs...>>>)
                 return Ψauto0_<I + 1, C>(mo(f));
  else return Ψauto__<C,
                      Tdrop<I, T<Xs...>>,
                      Ttake<I, T<Xs...>>>::f(mo(f));
}


template<bool C, class R, class... Xs>
auto Ψauto1_(F<R(Xs...)> &&f)
{
  if constexpr (Ψauto_is_done_<Xs...>::v) return f;
  else                                    return Ψauto0_<0, C>(mo(f));
}


template<bool C = false, class T>
auto Ψauto(T &&f)
{
  return Ψauto1_<C>(std::function(std::forward<T>(f)));
}


}

#include "end.hh"

#endif
