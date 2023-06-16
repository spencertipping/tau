#ifndef τπφmarkers_h
#define τπφmarkers_h

#include "types.hh"
#include "phi.hh"
#include "phi-ctor.hh"
#include "phi-auto.hh"
#include "pi-fn.hh"

#include "begin.hh"

namespace τ
{


template<chc *S> struct φaL {};             // auto literal
template<chc *S> struct φaO { Op<St> x; };  // auto optional
template<chc *S, bool N = false, u32 L = 0, u32 U = Nl<u32>::max()>
struct φaCs { St x; };  // auto charset

Tt struct πsa { using t = T; T x; };  // singular atom
Tt struct πpa { using t = T; T x; };  // plural atom
Tt struct πse { using t = T; T x; };  // singular expression
Tt struct πpe { using t = T; T x; };  // plural expression

Tt struct is_πv_         : std::false_type {};
Tt struct is_πv_<πsa<T>> : std::true_type {};
Tt struct is_πv_<πpa<T>> : std::true_type {};
Tt struct is_πv_<πse<T>> : std::true_type {};
Tt struct is_πv_<πpe<T>> : std::true_type {};

Tt concept is_πv = is_πv_<T>::value;


// Indicate that a value should be constructed at parse-time
Tt struct πP
{
  T       &operator*()       { return x; }
  T const &operator*() const { return x; }
  T x;
};

Tt struct is_πP_        : std::false_type {};
Tt struct is_πP_<πP<T>> : std::true_type {};

// While not technically πP derivatives, these parsers make sense only
// at parse time; they don't correspond to any runtime π values. No sense
// in requiring the user to wrap them with πP.
template<chc *S> struct is_πP_<φaL<S>> : std::true_type {};
template<chc *S> struct is_πP_<φaO<S>> : std::true_type {};
template<chc *S, bool N, u32 L, u32 U>
struct is_πP_<φaCs<S, N, L, U>> : std::true_type {};


// A marker to indicate that the π interpreter should reserve the specified
// number of bytes of memory in its heap before proceeding. This can force
// a GC to happen before any arguments are unpacked.
template<uN I> struct πhr_ { sletc s = I; };

Tt             struct is_πhr_          : std::false_type {};
template<uN I> struct is_πhr_<πhr_<I>> : std::true_type {};

Tt                    struct is_tuple_           : std::false_type {};
template<class... Xs> struct is_tuple_<T<Xs...>> : std::true_type {};


}

#include "end.hh"

#endif
