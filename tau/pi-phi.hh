#ifndef τπφ_h
#define τπφ_h

#include "phi.hh"
#include "phi-ctor.hh"
#include "pi-val.hh"

#include "begin.hh"

namespace τ
{


// Constructs a full π grammar using the provided elemental parsers:
//
// a = atom
// m = monadic operator (just the operator)
// d = dyadic operator
// t = ternary operator
//
// The resulting parser performs all folding and returns a single value.

φ<πf> πφ(φ<πf> a, φ<πmf> m, φ<πdf> d, φ<πtf> t);


// Built-in language elements
φ<St> πφws();
φ<St> πφlc();
φ<St> πφig();


template<class X>
φ<X> πφwrap(φ<X> p) { return φ2(φo(πφig()), p, φo(πφig())); }

template<class X>
φ<X> πφgroup(φ<X> p) { return φ2(πφwrap(φl("[", "")), πφwrap(p), πφwrap(φl("]", ""))); }


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
