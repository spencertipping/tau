#ifndef ττφ_h
#define ττφ_h

#include "types.hh"
#include "eta.hh"
#include "phi.hh"
#include "phi-ctor.hh"
#include "phi-auto.hh"
#include "begin.hh"

namespace τ
{


struct φident { St x; };
struct φword  { St x; };

Tt struct φrbrack { T x; };  // a thing terminated by a right-bracket
Tt struct φrparen { T x; };  // a thing terminated by a right-paren
Tt struct φrbrace { T x; };  // a thing terminated by a right-brace

struct φig {};  // ignored thing, e.g. whitespace/comments


// Built-in language elements
φ<St> φws_();  // whitespace
φ<St> φlc_();  // line comment
φ<St> φig_();  // ignored things (whitespace + line comments)

φ<St> φlb_();  // wrapped left bracket
φ<St> φrb_();  // wrapped right bracket
φ<St> φlp_();  // wrapped left paren
φ<St> φrp_();  // wrapped right paren
φ<St> φlB_();  // wrapped left brace
φ<St> φrB_();  // wrapped right brace

φ<St> φco_();  // wrapped comma


Tt φ<T> φwrap (φ<T> p) { return φ2("wrap", φig_(), p, φig_()); }
Tt φ<T> φgroup(φ<T> p) { return φ2("[]", φlb_(), φwrap(p), φrb_()); }
Tt φ<T> φparen(φ<T> p) { return φ2("()", φlp_(), φwrap(p), φrp_()); }
Tt φ<T> φbrace(φ<T> p) { return φ2("{}", φlB_(), φwrap(p), φrB_()); }


// Basic literals
φ<i64> φint_dec_();
φ<i64> φint_hex_();
φ<i64> φint_oct_();
φ<i64> φint_bin_();
φ<i64> φint_();  // selects from the above
φ<f64> φfloat_();

φ<St>     φstr_escape_();
φ<St>     φstr_();
φ<ηname>  φname_();
φ<φident> φident_();
φ<φword>  φword_();


// Parser packs for φ: these are bundles of parsers that can be used
// to construct a grammar. See phi-auto, pi-phi, and Gamma-phi for
// details.

struct φlit
{
  auto p(i64*) const { return φint_(); }
  auto p(i32*) const { return φm(p(null<i64>()), [](i64 x) { return i32(x); }); }
  auto p(i16*) const { return φm(p(null<i64>()), [](i64 x) { return i16(x); }); }
  auto p(f64*) const { return φfloat_(); }
  auto p(f32*) const { return φm(p(null<f64>()), [](f64 x) { return f32(x); }); }

  auto p(St*)     const { return φstr_(); }
  auto p(ηname*)  const { return φname_(); }
  auto p(φident*) const { return φident_(); }
  auto p(φword*)  const { return φword_(); }

  auto p(φig*) const { return φm(φig_(), [](auto&&) { return φig{}; }); }
};

struct φbrack
{
  Tt auto p(φrbrack<T>*) const
    { return φm(φ1("φ]", p(null<T>()), φrb_()), [](auto &&x) { return φrbrack{mo(x)}; }); }

  Tt auto p(φrparen<T>*) const
    { return φm(φ1("φ)", p(null<T>()), φrp_()), [](auto &&x) { return φrparen{mo(x)}; }); }

  Tt auto p(φrbrace<T>*) const
    { return φm(φ1("φ}", p(null<T>()), φrB_()), [](auto &&x) { return φrbrace{mo(x)}; }); }
};


}

#include "end.hh"

#endif
