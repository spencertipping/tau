#ifndef τπφ_h
#define τπφ_h

#include "phi.hh"
#include "phi-ctor.hh"
#include "phi-auto.hh"

#include "pi-fn.hh"

#include "begin.hh"

namespace τ
{


// Indicate that a value should be constructed at parse-time
Tt struct πP
{
  T       &operator*()       { return x; }
  T const &operator*() const { return x; }
  T x;
};

Tt struct is_πP_        : std::false_type {};
Tt struct is_πP_<πP<T>> : std::true_type {};


// TODO: a struct that splits an argument list into two packs, one for
// the parse-time arguments and one for the run-time arguments.


struct πφ final
{
  // TODO: design this in pi-phi.md first, then implement here
protected:
  φ<πf<1>> a_;      // atoms (alt)
  φ<πf<0>> spre_;   // singular prefix operators (dispatch)
  φ<πf<0>> spost_;  // singular postfix operators (dispatch)
  φ<πf<0>> ppre_;   // plural prefix operators (dispatch)
  φ<πf<0>> ppost_;  // plural postfix operators (dispatch)
};


// Take a single plural-element context and parse multiple of them,
// splicing the results into a single output η stream.
φ<πf<1>> πφfs(φ<πf<1>>);


// Built-in language elements
φ<St> πφword();  // an ident-like bareword
φ<St> πφws();    // whitespace
φ<St> πφlc();    // line comment
φ<St> πφig();    // ignored things (whitespace + line comments)

φ<St> πφlb();    // wrapped left bracket
φ<St> πφrb();    // wrapped right bracket

φ<St> πφlp();    // wrapped left paren
φ<St> πφrp();    // wrapped right paren


Tt φ<T> πφwrap (φ<T> p) { return φ2("wrap", πφig(), p, πφig()); }
Tt φ<T> πφgroup(φ<T> p) { return φ2("[]", πφlb(), πφwrap(p), πφrb()); }


// Basic literals
φ<i64> πφint_dec();
φ<i64> πφint_hex();
φ<i64> πφint_oct();
φ<i64> πφint_bin();
φ<i64> πφint();

φ<f64>   πφfloat();
φ<St>    πφstr_escape();
φ<St>    πφstr();
φ<ηname> πφname();


}

#include "end.hh"

#endif
