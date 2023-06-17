#ifndef τπauto_h
#define τπauto_h

#include "ctypes.hh"
#include "types.hh"
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
  using P = T<>;  // parseable things (const + immediate)
};

// NOTE: it's important to preserve ordering
template<πautometa X, class... Xs> struct πautoclassify_<X, Xs...>
{
  using M = typename Tcons_<T<X>, typename πautoclassify_<Xs...>::M>::t;
  using C = typename πautoclassify_<Xs...>::C;
  using I = typename πautoclassify_<Xs...>::I;
  using S = typename πautoclassify_<Xs...>::S;
  using P = typename πautoclassify_<Xs...>::P;
};

template<πautoconst X, class... Xs> struct πautoclassify_<X, Xs...>
{
  using M = typename πautoclassify_<Xs...>::M;
  using C = typename Tcons_<T<X>, typename πautoclassify_<Xs...>::C>::t;
  using I = typename πautoclassify_<Xs...>::I;
  using S = typename πautoclassify_<Xs...>::S;
  using P = typename Tcons_<T<X>, typename πautoclassify_<Xs...>::P>::t;

  // No meta args may come after const
  static_assert(std::tuple_size_v<M> == 0, "meta arg after const");
};

template<πautoimmed X, class... Xs> struct πautoclassify_<X, Xs...>
{
  using M = typename πautoclassify_<Xs...>::M;
  using C = typename πautoclassify_<Xs...>::C;
  using I = typename Tcons_<T<X>, typename πautoclassify_<Xs...>::I>::t;
  using S = typename πautoclassify_<Xs...>::S;
  using P = typename Tcons_<T<X>, typename πautoclassify_<Xs...>::P>::t;

  // No meta args may come after immed
  static_assert(std::tuple_size_v<M> == 0, "meta arg after immed");
};

template<πautostack X, class... Xs> struct πautoclassify_<X, Xs...>
{
  using M = typename πautoclassify_<Xs...>::M;
  using C = typename πautoclassify_<Xs...>::C;
  using I = typename πautoclassify_<Xs...>::I;
  using S = typename Tcons_<T<X>, typename πautoclassify_<Xs...>::S>::t;
  using P = typename πautoclassify_<Xs...>::P;

  // No const, meta, or immed args may come after stack
  static_assert(std::tuple_size_v<M> == 0, "meta arg after stack");
  static_assert(std::tuple_size_v<C> == 0, "const arg after stack");
  static_assert(std::tuple_size_v<I> == 0, "immed arg after stack");
};


// Fill a single native function argument -- it will be a meta, immediate,
// or stack (constants are provided by the parse closure). This means we need
// to match all types that will classify as such.
Tt struct πauto1_;

template<>     struct πauto1_<πi&>     { static πi  &f(πi &i) { return i; } };
template<>     struct πauto1_<πhr>     { static auto f(πi &i) { return i.pop(); } };
template<uN N> struct πauto1_<πhr_<N>> { static auto f(πi &i) { return πhr_<N>{}; } };

template<ηauto_decode T> struct πauto1_<T> { static auto f(πi &i) { return ηauto_<T>::v(i[i.pop()]); } };
template<is_πv        T> struct πauto1_<T> { static auto f(πi &i) { return T{πauto1_<typename T::T>::f(i)}; } };


// Fills arguments for an FFI function, drawing values from the parsed-constant
// closure list and the interpreter as necessary. The return value is not
// modified.
//
// Note that we must do this with imperative recursion as opposed to any type
// of unpacking-into-call-args because evaluation order matters.

template<uS I,         // destination function arg (FFI function)
         iN J,         // closure tuple position
         class R,      // function's return type
         class... Xs,  // function arg types
         class... Cs,  // closure value types
         class... Ys>  // transformed value types (converges to Xs...)

R πauto_apply_(F<R(Xs...)> const &f,
               T<Cs...>    const &t,
               πi &i,
               Ys&&... ys)  // NOTE: args collected here
{
  // Once all function args are satisfied, apply the function
  if constexpr (I == sizeof...(Xs)) return f(std::forward<Ys>(ys)...);
  else
  {
    using X = std::tuple_element_t<I, T<Xs...>>;
    if constexpr (πautoclass_<De<X>>::c == πautoclass::constant)
      return πauto_apply_<I + 1, J + 1>(
        f, t, i, std::forward<Ys>(ys)..., std::get<J>(t));
    else
      return πauto_apply_<I + 1, J>(
        f, t, i, std::forward<Ys>(ys)..., πauto1_<X>::f(i));
  }
}


// Return two tuples: one containing constants and the other containing
// immediates.
inline P<T<>, T<>> πauto_ci_split_(T<> const&) { return P<T<>, T<>>{}; }

template<class X, class... Xs>
P<typename πautoclassify_<X, Xs...>::C,
  typename πautoclassify_<X, Xs...>::I>
πauto_ci_split_(T<X, Xs...> const &t)
{
  auto [c, i] = πauto_ci_split_(tdrop(t));
  if constexpr (πautoclass_<De<X>>::c == πautoclass::constant)
    return {std::tuple_cat({std::get<0>(t)}, c), i};
  else
    return {c, std::tuple_cat({std::get<0>(t)}, i)};
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


// Construct a πf<N> from an inner FFI function and a tuple of closure values.
template<class R, class... Xs, class... Cs>
auto πauto_(Stc &n, F<R(Xs...)> const &f, T<Cs...> &&t)
{
  // Stack delta = returned values - stack inputs
  sletc rvals = int(πvrauto_<R>::n);
  sletc svals = int(std::tuple_size_v<typename πautoclassify_<Xs...>::S>);
  return πf<rvals - svals>{n, [t=mo(t), f](πi &i)
    { if constexpr (Eq<R, void>) πauto_apply_<0, 0>(f, t, i);
      else  πvrauto_<R>::push(i, πauto_apply_<0, 0>(f, t, i)); }};
}


// Push immediate arguments in reverse order onto the stack, in this case by
// inlining each.
template<iN I, class... Xs>
auto πauto_ipush_(T<Xs...> const &t)
{
  if constexpr (I < 0) return π0{};
  else                 return std::get<I>(t) | πauto_ipush_<I - 1>(tdrop(t));
}


// Generate a parser for a tuple of types
// NOTE: return type is not φ<T<Xs...>> because the parsers may be immediate π1
template<class A, class... Xs>
auto πauto_parser_(A const &a, Stc &n, T<Xs...> const&)
{ return φs(n, a.p(std::declval<De<Xs>>())...); }


// Convert without parsing: this converts C++ into πf<N>
template<class F>
auto πauto(Stc &n, F const &f) { return πauto_(n, std::function(f), T<>{}); }


// Convert a C++ FFI function to a parser that generates all constants and
// returns a πf<N> that produces intermediates and then pops/decodes all values
// into the wrapped function.
template<class A, class... Xs, class R>
auto πauto(A const &a, Stc &n, F<R(Xs...)> const &f)
{
  using Cl = πautoclassify_<Xs...>;
  using P  = typename Cl::P;
  using I  = typename Cl::I;
  sletc N  = iN(std::tuple_size_v<I>);
  return φm(πauto_parser_(a, n, std::declval<P>()),
            [n, f](P &&t)
              { auto [c, i] = πauto_ci_split_(t);
                return πauto_ipush_<N - 1>(i) | πauto_(n, f, mo(c)); });
}


}

#include "end.hh"

#endif
