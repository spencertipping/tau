#ifndef τπφ_h
#define τπφ_h

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


// Indicate that a value should be constructed at parse-time
Tt struct πP
{
  T       &operator*()       { return x; }
  T const &operator*() const { return x; }
  T x;
};

Tt struct is_πP_        : std::false_type {};
Tt struct is_πP_<πP<T>> : std::true_type {};


template<class... Xs> struct πPsplit_;

template<> struct πPsplit_<>
{
  using P = T<>;  // parse-time arguments
  using R = T<>;  // runtime arguments
};

template<class X, class... Xs> struct πPsplit_<X, Xs...>
{
  using P = typename Co<is_πP_<X>::value,
                        typename Tcons_<T<X>, typename πPsplit_<Xs...>::P>::t,
                        typename πPsplit_<Xs...>::R>::t;
  using R = typename Co<is_πP_<X>::value,
                        typename πPsplit_<Xs...>::P,
                        typename Tcons_<T<X>, typename πPsplit_<Xs...>::R>::t>::t;
};


template<class P, class R> struct πPsplit__;
template<class... Ps, class... Rs> struct πPsplit__<T<Ps...>, T<Rs...>>
{
  Tt static auto f(St n, F<T(Ps..., Rs...)> &&f)
    { return [n, f=mo(f)](Ps&&... ps)
      { return πvauto(n, [=, f=mo(f)](Rs&&... rs)
        { return f(ps..., rs...); }); }; }
};


// Split a function and prepare it to be used with φauto. This is called by
// πφ when defining grammar elements.
template<class T, class... Xs>
auto πPsplit(St n, F<T(Xs...)> &&f)
{
  return πPsplit__<typename πPsplit_<Xs...>::P,
                   typename πPsplit_<Xs...>::R>::f(n, f);
}


// An extensible π grammar. See pi-phi.md for details.
struct πφ final
{
  πφ() : s_  (φa0<πf<1>>("πs")),
         p_  (φa0<πf<1>>("πp")),
         ssp_(φd <πf<0>>("πssp")),
         psp_(φd <πf<0>>("πpsp")),
         spp_(φd <πf<0>>("πspp")),
         ppp_(φd <πf<0>>("πppp"))
    {
      // TODO: assemble toplevel parsers here
    }


  πφ &def_sa(φ<πf<1>> p) { s_.as<φa_<πf<1>>>() << p; return *this; }
  πφ &def_pa(φ<πf<1>> p) { p_.as<φa_<πf<1>>>() << p; return *this; }

  Tt πφ &def_ssp(St n, T f) { return def_(ssp_, n, f); }
  Tt πφ &def_psp(St n, T f) { return def_(psp_, n, f); }
  Tt πφ &def_spp(St n, T f) { return def_(spp_, n, f); }
  Tt πφ &def_ppp(St n, T f) { return def_(ppp_, n, f); }
  Tt πφ &def_sp (St n, T f) { return def_(sp_,  n, f); }
  Tt πφ &def_pp (St n, T f) { return def_(pp_,  n, f); }


  Tt auto p(πP<T>&&) const
    { return φm(p(std::declval<T>()), [](T &&x) { return πP<T>{x}; }); }

  auto p(i64&&)   const { return πφint(); }
  auto p(f64&&)   const { return πφfloat(); }
  auto p(St&&)    const { return πφstr(); }
  auto p(ηname&&) const { return πφname(); }


protected:
  Tt πφ &def_(φ<πf<0>> &d, St n, T f)
    { let p = φauto(*this, πPsplit(n, std::function(f)));
      d.as<φd_<πf<0>>>().def(n, p);
      return *this; }

  // TODO: define toplevel parsers

  φ<πf<1>> s_;    // singular atoms (alt)
  φ<πf<1>> p_;    // plural atoms (alt)
  φ<πf<0>> ssp_;  // singular-operand, singular-return prefix (dispatch)
  φ<πf<0>> psp_;  // plural-operand, singular-return prefix (dispatch)
  φ<πf<0>> spp_;  // singular-operand, plural-return prefix (dispatch)
  φ<πf<0>> ppp_;  // plural-operand, plural-return prefix (dispatch)
  φ<πf<0>> sp_;   // singular → singular postfix (dispatch)
  φ<πf<0>> pp_;   // plural → plural postfix (dispatch)
};


}

#include "end.hh"

#endif
