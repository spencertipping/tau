#ifndef τπφ_h
#define τπφ_h

#include "phi.hh"
#include "phi-ctor.hh"
#include "phi-auto.hh"

#include "begin.hh"

namespace τ
{


struct πf {};
struct πfs {};

struct πname { St x; };


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

// TODO: this struct should be generated from a prefix dict so we can
// more easily extend the grammar

// TODO: πφ can use templates to implement stateful φauto for its
// constituent functions; this way we sidestep the global-parser problem

// TODO: write an example π operator function and define our expectations
// about how its arguments would be auto-filled
//
// NOTE: we need one-off .def("prefix", []() {}) calls in order to
// accommodate pre-auto parse template functions; some of the args to these
// functions will be πf or πfs, which will be filled by s and p from πφ.

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


// Take a single plural-element context and parse multiple of them,
// splicing the results into a single output list
φ<πfs> πφfs(φ<πfs>);


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
φ<πname> πφname();


}

#include "end.hh"

#endif
