#ifndef τπφ_h
#define τπφ_h

#include "phi.hh"
#include "phi-ctor.hh"
#include "pi-val.hh"

#include "begin.hh"

namespace τ
{


// Core language, for testing/example
φ<πf>  πφcore_a();
φ<πmf> πφcore_m();
φ<πdf> πφcore_d();
φ<πtf> πφcore_t();


// Constructs a full π grammar using the provided elemental parsers:
//
// a = atom
// m = monadic operator (just the operator)
// d = dyadic operator
// t = ternary operator
//
// The resulting parser performs all folding and returns a single value.

φ<πf> πφ(φ<πf>  a = πφcore_a(),
         φ<πmf> m = πφcore_m(),
         φ<πdf> d = πφcore_d(),
         φ<πtf> t = πφcore_t());


// Built-in language elements
φ<St> πφws();
φ<St> πφlc();
φ<St> πφig();

φ<St> πφlb();
φ<St> πφrb();

φ<St> πφlp();
φ<St> πφrp();


template<class X>
φ<X> πφwrap(φ<X> p) { return φ2("wrap", πφig(), p, πφig()); }

template<class X>
φ<X> πφgroup(φ<X> p) { return φ2("[]", πφlb(), πφwrap(p), πφrb()); }

template<class X>
φ<V<X>> πφtuple(φ<X> p) { return φ2("()", πφlp(), φn(πφwrap(p)), φo(πφrp())); }


// Basic literals
φ<i64> πφint_dec();
φ<i64> πφint_hex();
φ<i64> πφint_oct();
φ<i64> πφint_bin();
φ<i64> πφint();

φ<f64>   πφfloat();
φ<πname> πφname();
φ<St>    πφstr_escape();
φ<St>    πφstr();

φ<πv> πφlit();


}

#include "end.hh"

#endif
