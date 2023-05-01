#ifndef τπφ_h
#define τπφ_h

#include "phi.hh"
#include "phi-ctor.hh"
#include "phi-auto.hh"
#include "pi-val.hh"

#include "begin.hh"

namespace τ
{


// Core language, for testing/example
φ<πf>  πφcore_a();
φ<πmf> πφcore_m();
φ<πdf> πφcore_d();
φ<πtf> πφcore_t();

φ<πmsf> πφcore_ms();
φ<πdsf> πφcore_ds();
φ<πtsf> πφcore_ts();

φ<πsmf>  πφcore_sm();
φ<πsmsf> πφcore_sms();


// Constructs a full π grammar using the provided elemental parsers:
//
// a = atom
// m = monadic operator (just the operator)
// d = dyadic operator
// t = ternary operator
//
// ms = monadic plural operator
// ds = dyadic plural operator
// ts = ternary plural operator
//
// sm = monadic operator consuming plural values
// sms = monadic operator producing and consuming plural values
//
// πφ returns the following parser objects:
//
// 1. A parser for atoms; i.e. `[]` is required around any operators
// 2. A parser for single-valued expressions
// 3. A parser for multi-valued expressions

T<φ<πf>, φ<πf>, φ<πfs>>
πφ(φ<πf>  a = πφcore_a(),
   φ<πmf> m = πφcore_m(),
   φ<πdf> d = πφcore_d(),
   φ<πtf> t = πφcore_t(),

   φ<πmsf> ms = πφcore_ms(),
   φ<πdsf> ds = πφcore_ds(),
   φ<πtsf> ts = πφcore_ts(),

   φ<πsmf>  sm  = πφcore_sm(),
   φ<πsmsf> sms = πφcore_sms());


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
