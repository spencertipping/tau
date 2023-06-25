#ifndef τπφbasic_h
#define τπφbasic_h

#include "types.hh"
#include "phi.hh"
#include "phi-ctor.hh"
#include "phi-auto.hh"
#include "pi-phi-markers.hh"
#include "pi-fn.hh"

#include "begin.hh"

namespace τ
{


// Take a single plural-element context and parse multiple of them,
// splicing the results into a single output η stream.
φ<π1> πφnp(φ<π1>);


// TODO: factor these generic things into a separate file

struct πident { St x; };

Tt struct πrbrack { T x; };  // a thing terminated by a right-bracket
Tt struct πrparen { T x; };  // a thing terminated by a right-paren
Tt struct πrbrace { T x; };  // a thing terminated by a right-brace


// Built-in language elements
φ<St>     πφword();   // an ident-like bareword (like a map key)
φ<πident> πφident();  // an ident, specifically (like a variable)

φ<St> πφws();  // whitespace
φ<St> πφlc();  // line comment
φ<St> πφig();  // ignored things (whitespace + line comments)

φ<St> πφlb();  // wrapped left bracket
φ<St> πφrb();  // wrapped right bracket
φ<St> πφlp();  // wrapped left paren
φ<St> πφrp();  // wrapped right paren
φ<St> πφlB();  // wrapped left brace
φ<St> πφrB();  // wrapped right brace

φ<St> πφco();  // wrapped comma


Tt φ<T> πφwrap (φ<T> p) { return φ2("wrap", πφig(), p, πφig()); }
Tt φ<T> πφgroup(φ<T> p) { return φ2("[]", πφlb(), πφwrap(p), πφrb()); }
Tt φ<T> πφparen(φ<T> p) { return φ2("()", πφlp(), πφwrap(p), πφrp()); }
Tt φ<T> πφbrace(φ<T> p) { return φ2("{}", πφlB(), πφwrap(p), πφrB()); }


// Basic literals
φ<i64> πφint_dec();
φ<i64> πφint_hex();
φ<i64> πφint_oct();
φ<i64> πφint_bin();
φ<i64> πφint();  // selects from the above

φ<f64>   πφfloat();
φ<St>    πφstr_escape();
φ<St>    πφstr();
φ<ηname> πφname();


// Parser packs for πφ: these are bundles of parsers that can be used
// to construct a grammar.

struct πφP
{
  Tt auto p(πP<T>*) const
    { return φm(p(null<T>()), [](auto &&x) { return πP{x}; }); }
};

struct πφlit
{
  auto p(i64*) const { return πφint(); }
  auto p(i32*) const { return φm(p(null<i64>()), [](i64 x) { return i32(x); }); }
  auto p(i16*) const { return φm(p(null<i64>()), [](i64 x) { return i16(x); }); }
  auto p(f64*) const { return πφfloat(); }
  auto p(f32*) const { return φm(p(null<f64>()), [](f64 x) { return f32(x); }); }

  auto p(St*)     const { return πφstr(); }
  auto p(ηname*)  const { return πφname(); }
  auto p(πident*) const { return πφident(); }
};

struct πφbrack
{
  Tt auto p(πrbrack<T>*) const
    { return φm(φ1("π]", p(null<T>()), πφrb()), [](auto &&x) { return πrbrack{mo(x)}; }); }

  Tt auto p(πrparen<T>*) const
    { return φm(φ1("π)", p(null<T>()), πφrp()), [](auto &&x) { return πrparen{mo(x)}; }); }

  Tt auto p(πrbrace<T>*) const
    { return φm(φ1("π}", p(null<T>()), πφrB()), [](auto &&x) { return πrbrace{mo(x)}; }); }
};


}

#include "end.hh"

#endif
