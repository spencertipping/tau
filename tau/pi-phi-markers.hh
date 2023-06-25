#ifndef τπφmarkers_h
#define τπφmarkers_h

#include "types.hh"
#include "phi.hh"
#include "phi-ctor.hh"
#include "phi-auto.hh"
#include "phi-markers.hh"
#include "pi-fn.hh"

#include "begin.hh"

namespace τ
{


Tt struct πsa { using t = T; T x; };  // singular atom
Tt struct πpa { using t = T; T x; };  // plural atom
Tt struct πse { using t = T; T x; };  // singular expression
Tt struct πpe { using t = T; T x; };  // plural expression

Tt struct πst { using t = T; T x; };  // singular transformer
Tt struct πpt { using t = T; T x; };  // plural transformer

Tt struct is_πv_         : std::false_type {};
Tt struct is_πv_<πsa<T>> : std::true_type {};
Tt struct is_πv_<πpa<T>> : std::true_type {};
Tt struct is_πv_<πse<T>> : std::true_type {};
Tt struct is_πv_<πpe<T>> : std::true_type {};
Tt struct is_πv_<πst<T>> : std::true_type {};
Tt struct is_πv_<πpt<T>> : std::true_type {};

Tt concept is_πv = is_πv_<T>::value;


// Indicate that a value should be constructed at parse-time.
Tt struct πP
{
  T       &operator*()       { return x; }
  T const &operator*() const { return x; }
  T x;
};

Tt struct is_πP_        : std::false_type {};
Tt struct is_πP_<πP<T>> : std::true_type {};


// A marker to indicate that the π interpreter should reserve the specified
// number of bytes of memory in its heap before proceeding. This can force
// a GC to happen before any arguments are unpacked.
template<uN I> struct πhr_ { sletc s = I; };

Tt             struct is_πhr_          : std::false_type {};
template<uN I> struct is_πhr_<πhr_<I>> : std::true_type {};


Tt O &operator<<(O &s, πsa<T> const &x) { return s << x.x; }
Tt O &operator<<(O &s, πpa<T> const &x) { return s << x.x; }
Tt O &operator<<(O &s, πse<T> const &x) { return s << x.x; }
Tt O &operator<<(O &s, πpe<T> const &x) { return s << x.x; }

Tt O &operator<<(O &s, πst<T> const &x) { return s << x.x; }
Tt O &operator<<(O &s, πpt<T> const &x) { return s << x.x; }

Tt O &operator<<(O &s, πP<T>  const &x) { return s << x.x; }


}

#include "end.hh"

#endif
