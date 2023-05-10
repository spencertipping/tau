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
//
// NOTE: you must hold references to all returned parsers for any of
// them to be valid

// FIXME TODO: πφ requires stateful initialization, which is a problem
// for φauto because we don't have a simple way to specify function args
// for auto-filled parameters. This type of grammar should be a struct
// with template args that provide dependencies.
//
// This also fixes the hold-onto-all-tuple-elements issue.

// NOTE: the new structure here should be two parsers: φ<πf> for atoms
// and F<φ<F<πf(πf)>>(φ<πf>)> for right-hand operators. These should be
// struct members that are then fed into a virtual grammar constructor
// and are initialized by derived classes.

φ<πfs> πφfs(φ<πfs> f);

struct πφ final
{
  πφ(φ<πf>,
     F<φ<F<πf (πf)>>(φ<πf>, φ<πfs>)> const&,
     F<φ<F<πfs(πf)>>(φ<πf>, φ<πfs>)> const&);

  φ<F<πf (πf)>> os;
  φ<F<πfs(πf)>> op;
  φ<πf>         a;
  φ<πf>         s;
  φ<πfs>        p;
};


T<φ<πf>, φ<πf>, φ<πfs>>
πφ_(φ<πf>  a = πφcore_a(),
   φ<πmf> m = πφcore_m(),
   φ<πdf> d = πφcore_d(),
   φ<πtf> t = πφcore_t(),

   φ<πmsf> ms = πφcore_ms(),
   φ<πdsf> ds = πφcore_ds(),
   φ<πtsf> ts = πφcore_ts(),

   φ<πsmf>  sm  = πφcore_sm(),
   φ<πsmsf> sms = πφcore_sms());


// Built-in language elements
φ<St> πφword();
φ<St> πφws();
φ<St> πφlc();
φ<St> πφig();

φ<St> πφlb();
φ<St> πφrb();

φ<St> πφlp();
φ<St> πφrp();


Tt φ<T> πφwrap (φ<T> p) { return φ2("wrap", πφig(), p, πφig()); }
Tt φ<T> πφgroup(φ<T> p) { return φ2("[]", πφlb(), πφwrap(p), πφrb()); }


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
