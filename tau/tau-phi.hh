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


O &operator<<(O&, φig);
O &operator<<(O&, φident const&);
O &operator<<(O&, φword  const&);

Tt O &operator<<(O &s, φrbrack<T> const& x) { return s << "φrbrack" << x.x; }
Tt O &operator<<(O &s, φrparen<T> const& x) { return s << "φrparen" << x.x; }
Tt O &operator<<(O &s, φrbrace<T> const& x) { return s << "φrbrace" << x.x; }


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

template<class A> struct φlit
{
  auto p(u64*) const { return φint_() * [](i64 x) { return u64(x); }; }
  auto p(i64*) const { return φint_(); }
  auto p(i32*) const { return φm(Sc<A const*>(this)->p(null<i64>()), [](i64 x) { return i32(x); }); }
  auto p(i16*) const { return φm(Sc<A const*>(this)->p(null<i64>()), [](i64 x) { return i16(x); }); }
  auto p(f64*) const { return φfloat_(); }
  auto p(f32*) const { return φm(Sc<A const*>(this)->p(null<f64>()), [](f64 x) { return f32(x); }); }

  auto p(St*)     const { return φstr_(); }
  auto p(ηname*)  const { return φname_(); }
  auto p(φident*) const { return φident_(); }
  auto p(φword*)  const { return φword_(); }

  auto p(V<St>*) const
    { let each = φm(φident_(), [](auto &&x) { return x.x; }) | φstr_();
      return φ2("[s*]", φlb_(), φn(φwrap(each)), φrb_()); }

  auto p(φig*) const { return φm(φig_(), [](auto&&) { return φig{}; }); }

  auto p(bytes*) const
    { return φauto(*Rc<A const*>(this), [](f64 x, byte_suffix y) { return bytes{u64(x * u64(y))}; }); }

  auto p(byte_suffix*) const
    { return φd<byte_suffix>("byte_suffix",
                             "",   φR(byte_suffix::one),
                             "B",  φR(byte_suffix::one),
                             "K",  φR(byte_suffix::kilo),
                             "Ki", φR(byte_suffix::kibi),
                             "M",  φR(byte_suffix::mega),
                             "Mi", φR(byte_suffix::mebi),
                             "G",  φR(byte_suffix::giga),
                             "Gi", φR(byte_suffix::gibi),
                             "T",  φR(byte_suffix::tera),
                             "Ti", φR(byte_suffix::tebi),
                             "P",  φR(byte_suffix::peta),
                             "Pi", φR(byte_suffix::pebi),
                             "E",  φR(byte_suffix::exa),
                             "Ei", φR(byte_suffix::exbi)); }
};


template<class A> struct φbrack
{
  Tt auto p(φrbrack<T>*) const
    { return φm(φ1("φ]", Sc<A const*>(this)->p(null<T>()), φrb_()), [](T &&x) { return φrbrack<T>{mo(x)}; }); }

  Tt auto p(φrparen<T>*) const
    { return φm(φ1("φ)", Sc<A const*>(this)->p(null<T>()), φrp_()), [](T &&x) { return φrparen<T>{mo(x)}; }); }

  Tt auto p(φrbrace<T>*) const
    { return φm(φ1("φ}", Sc<A const*>(this)->p(null<T>()), φrB_()), [](T &&x) { return φrbrace<T>{mo(x)}; }); }
};


}

#include "end.hh"

#endif
