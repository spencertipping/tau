#ifndef τφmarkers_h
#define τφmarkers_h

#include "types.hh"
#include "begin.hh"

namespace τ
{


template<u64 C>     struct φaU { using t = u64;    t x; };  // auto unicode codepoint
template<char... S> struct φaL { using t = St;     t x; };  // auto literal
template<char... S> struct φaO { using t = Op<St>; t x; };  // auto optional
template<bool N = false, u32 L = 0, u32 U = Nl<u32>::max(), char... S>
struct φaCs { using t = St; t x; };  // auto charset

template<class X, u32 L = 0, u32 U = Nl<u32>::max()>
struct φaN { using t = V<X>; t x; };

Txs struct φaS { using t = T<Xs...>; t x; };


Txs  struct φaA;    // auto alternation (all of same type)
Txxs struct φaA<X, Xs...>
{ using t = typename X::t; t x; };


// Detect instances of the above parser-arguments
Tt struct is_φa_ : std::false_type {};
template<u64 C>     struct is_φa_<φaU<C>>    : std::true_type {};
template<char... S> struct is_φa_<φaL<S...>> : std::true_type {};
template<char... S> struct is_φa_<φaO<S...>> : std::true_type {};
template<char... S, bool N, u32 L, u32 U> struct is_φa_<φaCs<N, L, U, S...>> : std::true_type {};

Txs                             struct is_φa_<φaA<Xs...>>   : std::true_type {};
Txs                             struct is_φa_<φaS<Xs...>>   : std::true_type {};
template<class X, u32 L, u32 U> struct is_φa_<φaN<X, L, U>> : std::true_type {};

Tt concept is_φa = is_φa_<T>::value;


template<u64 C>     O &operator<<(O &s, φaU<C>     const &x) { return s << "#U" << C; }
template<char... S> O &operator<<(O &s, φaL<S...>  const &x) { return s << "'" << St{S...} << "'"; }
template<char... S> O &operator<<(O &s, φaO<S...>  const &x) { return s << "'" << St{S...} << "'?"; }
Txs                 O &operator<<(O &s, φaA<Xs...> const &x) { return s << "φaA(" << x.x << ")"; }

template<class X, u32 L, u32 U> O &operator<<(O &s, φaN<X, L, U> const &x) { return s << "φaN(...)"; }
Txs                             O &operator<<(O &s, φaS<Xs...>   const &x) { return s << "φaS(...)"; }


}

#include "end.hh"

#endif
