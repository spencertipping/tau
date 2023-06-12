#ifndef τπφbasic_h
#define τπφbasic_h

#include "types.hh"
#include "phi.hh"
#include "phi-ctor.hh"
#include "phi-auto.hh"
#include "pi-fn.hh"

#include "begin.hh"

namespace τ
{


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
