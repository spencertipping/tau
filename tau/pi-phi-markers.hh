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


Tt struct πt  { using t = T; T x; };  // π toplevel program
Tt struct πs  { using t = T; T x; };
Tt struct πp  { using t = T; T x; };
Tt struct πsl { using t = T; T x; };
Tt struct πsr { using t = T; T x; };
Tt struct πpl { using t = T; T x; };
Tt struct πpr { using t = T; T x; };

Tt using πtc  = πt<T> const;
Tt using πsc  = πs<T> const;
Tt using πpc  = πp<T> const;
Tt using πslc = πsl<T> const;
Tt using πsrc = πsr<T> const;
Tt using πplc = πpl<T> const;
Tt using πprc = πpr<T> const;

Tt struct is_πv_         : std::false_type {};
Tt struct is_πv_<πt<T>>  : std::true_type {};
Tt struct is_πv_<πs<T>>  : std::true_type {};
Tt struct is_πv_<πp<T>>  : std::true_type {};
Tt struct is_πv_<πsl<T>> : std::true_type {};
Tt struct is_πv_<πpl<T>> : std::true_type {};

Tt struct is_πf_         : std::false_type {};
Tt struct is_πf_<πsr<T>> : std::true_type {};
Tt struct is_πf_<πpr<T>> : std::true_type {};

Tt concept is_πv = is_πv_<T>::value;
Tt concept is_πf = is_πf_<T>::value;


// Indicate that a value should be constructed at parse-time.
Tt struct πP
{
  T       &operator*()       { return x; }
  T const &operator*() const { return x; }
  T x;
};

Tt using πPc = πP<T> const;

Tt πP(T) -> πP<T>;

Tt struct is_πP_        : std::false_type {};
Tt struct is_πP_<πP<T>> : std::true_type {};


// Indicate that a value should be extracted from the stack.
Tt struct πS { using t = T; T x; };
Tt πS(T) -> πS<T>;

Tt using πSc = πS<T> const;


// A marker to indicate that the π interpreter should reserve the specified
// number of bytes of memory in its heap before proceeding. This can force
// a GC to happen before any arguments are unpacked.
template<uN I> struct πhr_ { sletc s = I; };

Tt             struct is_πhr_          : std::false_type {};
template<uN I> struct is_πhr_<πhr_<I>> : std::true_type {};


Tt O &operator<<(O &s, πt <T> const &x) { return s << x.x; }
Tt O &operator<<(O &s, πs<T>  const &x) { return s << x.x; }
Tt O &operator<<(O &s, πp<T>  const &x) { return s << x.x; }
Tt O &operator<<(O &s, πsl<T> const &x) { return s << x.x; }
Tt O &operator<<(O &s, πsr<T> const &x) { return s << x.x; }
Tt O &operator<<(O &s, πpl<T> const &x) { return s << x.x; }
Tt O &operator<<(O &s, πpr<T> const &x) { return s << x.x; }

Tt O &operator<<(O &s, πP<T>  const &x) { return s << x.x; }


}

#include "end.hh"

#endif
