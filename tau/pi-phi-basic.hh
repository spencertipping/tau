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
Tt φ<T> πφparen(φ<T> p) { return φ2("()", πφlp(), πφwrap(p), πφrp()); }


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


// Parser packs for πφ: these are bundles of parsers that can be used
// to construct a grammar.

struct πφP
{
  Tt auto p(πP<T>&&) const
    { return φm(p(std::declval<T>()), [](T &&x) { return πP<T>{x}; }); }
};

struct πφlit
{
  auto p(i64&&)   const { return πφint(); }
  auto p(f64&&)   const { return πφfloat(); }
  auto p(St&&)    const { return πφstr(); }
  auto p(ηname&&) const { return πφname(); }
};

struct πφstr
{
  template<chc *S> auto p(φaL<S>&&) const
    { return φm(φl(St{S}), [](auto &&x) { return φaL<S>{mo(x)}; }); }

  template<chc *S> auto p(φaO<S>&&) const
    { return φm(φo(φl(St{S})), [](auto &&x) { return φaCs<S>{mo(x)}; }); }

  template<chc *S, bool N, u32 L, u32 U> auto p(φaCs<S, N, L, U>&&) const
    { return φm(φcs(S, N, L, U),
                [](auto &&x) { return φaCs<S, N, L, U>{mo(x)}; }); }

  template<class... Xs> auto p(φaA<Xs...>&&) const
    { return φm(φa(p(std::declval<Xs>())...),
                [](auto &&x) { return φaA<Xs...>{mo(x)}; }); }
};


}

#include "end.hh"

#endif
