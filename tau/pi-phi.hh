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
// TODO: port these to the new parser structure

φ<πf>  πφcore_a();
φ<πmf> πφcore_m();
φ<πdf> πφcore_d();
φ<πtf> πφcore_t();

φ<πmsf> πφcore_ms();
φ<πdsf> πφcore_ds();
φ<πtsf> πφcore_ts();

φ<πsmf>  πφcore_sm();
φ<πsmsf> πφcore_sms();


φ<πfs> πφfs(φ<πfs> f);


// π grammar generated from core parsers, written here in function
// notation:
//
// φ<πf>: an individual atom
// (φ<πf>, φ<πfs>) → φ<πf>: singular prefix operator
// (φ<πf>, φ<πfs>) → φ<πfs>: plural prefix operator
// (φ<πf>, φ<πfs>) → φ<πf → πf>: singular postfix operator
// (φ<πf>, φ<πfs>) → φ<πf → πfs>: plural postfix operator
//
// Here, φ<πf> is the compiled singular-expression parser and φ<πfs> is
// the compiled plural-expression parser. In the returned φs, πf → x
// means "take the left-hand operand and return the full expression";
// these parsers consume the continuation after the left operand.
//
// At every position in the source, at most one of the five parsers
// should accept any continuation. If more than one accepts, then the
// grammar is ambiguous and depends on preferential ordering (which may
// be acceptable, but isn't ideal).

struct πφ final
{
  πφ(φ<πf>,
     F<φ<πf>        (φ<πf>, φ<πfs>)> const&,
     F<φ<πfs>       (φ<πf>, φ<πfs>)> const&,
     F<φ<F<πf (πf)>>(φ<πf>, φ<πfs>)> const&,
     F<φ<F<πfs(πf)>>(φ<πf>, φ<πfs>)> const&);

  φ<F<πf (πf)>> os;  // compiled postfix singular
  φ<F<πfs(πf)>> op;  // compiled postfix plural
  φ<πf>         es;  // compiled prefix singular
  φ<πfs>        ep;  // compiled prefix plural
  φ<πf>         a;   // atom
  φ<πf>         s;   // singular expression
  φ<πfs>        p;   // plural expression
};


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
