#ifndef τφmarkers_h
#define τφmarkers_h

#include "types.hh"
#include "begin.hh"

namespace τ
{


template<char... S> struct φaL { using t = St;     t x; };  // auto literal
template<char... S> struct φaO { using t = Op<St>; t x; };  // auto optional
template<bool N = false, u32 L = 0, u32 U = Nl<u32>::max(), char... S>
struct φaCs { using t = St; t x; };  // auto charset

template<class... Xs> struct φaA;    // auto alternation
template<class X, class... Xs> struct φaA<X, Xs...>
{ using t = typename X::t; t x; };


// Detect instances of the above parser-arguments
Tt struct is_φa_ : std::false_type {};
template<char... S> struct is_φa_<φaL<S...>> : std::true_type {};
template<char... S> struct is_φa_<φaO<S...>> : std::true_type {};
template<char... S, bool N, u32 L, u32 U> struct is_φa_<φaCs<N, L, U, S...>> : std::true_type {};
template<class... Xs> struct is_φa_<φaA<Xs...>> : std::true_type {};

Tt concept is_φa = is_φa_<T>::value;


template<char... S>   O &operator<<(O &s, φaL<S...>  const &x) { return s << "'" << St{S...} << "'"; }
template<char... S>   O &operator<<(O &s, φaO<S...>  const &x) { return s << "'" << St{S...} << "'?"; }
template<class... Xs> O &operator<<(O &s, φaA<Xs...> const &x) { return s << "φaA(" << x.x << ")"; }


}

#include "end.hh"

#endif
