#ifndef τΓφ_h
#define τΓφ_h

#include "phi.hh"
#include "phi-ctor.hh"
#include "Gamma.hh"
#include "Gamma-auto.hh"
#include "Psi.hh"
#include "Psi-auto.hh"
#include "pi-phi.hh"

#include "begin.hh"

namespace τ
{


Tt struct Γa { T x; };
Tt struct Γe { T x; };


// An extensible Γ grammar. See pi-phi.hh for a similar construct for π.
template<class Γφ, class πφ>
struct Γφ_
{
  Γφ_();
  virtual ~Γφ_() {}

  Γφ       &self()       { return *Rc<Γφ*>(this); }
  Γφ const &self() const { return *Rc<Γφ const*>(this); }

  πφ       &pi()         { return pf_; }
  πφ const &pi()   const { return pf_; }


  Tt Γφ &def_g (Stc &n, T const &f) { return def_(g_,  n, Γauto(f)); }

  Tt Γφ &def_p0(Stc &n, T const &f) { return def_(p0_, n, Ψauto<false>(f)); }
  Tt Γφ &def_p1(Stc &n, T const &f) { return def_(p1_, n, Ψauto<false>(f)); }
  Tt Γφ &def_p2(Stc &n, T const &f) { return def_(p2_, n, Ψauto<false>(f)); }
  Tt Γφ &def_p4(Stc &n, T const &f) { return def_(p4_, n, Ψauto<false>(f)); }

  Tt Γφ &def_c0(Stc &n, T const &f) { return def_(p0_, n, Ψauto<true>(f)); }
  Tt Γφ &def_c1(Stc &n, T const &f) { return def_(p1_, n, Ψauto<true>(f)); }
  Tt Γφ &def_c2(Stc &n, T const &f) { return def_(p2_, n, Ψauto<true>(f)); }
  Tt Γφ &def_c4(Stc &n, T const &f) { return def_(p4_, n, Ψauto<true>(f)); }


  φ<Γ> ta() const { return wa_; }
  φ<Γ> te() const { return we_; }

  // Parse a Γ program and return its parse result, which can be either
  // success or failure. To assume success, use operator().
  φr_<Γ> parse(Stc &s) const { return wee_(s); }
  Γ operator()(Stc &s) const { return parse(s).r(); }


  auto p(Γa<Γ>*) const { return φm(wa_, [](Γ x) { return Γa<Γ>{x}; }); }
  auto p(Γe<Γ>*) const { return φm(we_, [](Γ x) { return Γe<Γ>{x}; }); }

  auto p(Γ*)  const { return wa_; }
  auto p(Ψ0*) const { return wp0_; }
  auto p(Ψ1*) const { return wp1_; }
  auto p(Ψ2*) const { return wp2_; }
  auto p(Ψ4*) const { return wp4_; }


  Tt auto p(πt <T> *x) const { return pf_.p(x); }
  Tt auto p(πsa<T> *x) const { return pf_.p(x); }
  Tt auto p(πpa<T> *x) const { return pf_.p(x); }
  Tt auto p(πse<T> *x) const { return pf_.p(x); }
  Tt auto p(πpe<T> *x) const { return pf_.p(x); }

  // Parse-time evaluation of a π expression
  φ<ηm> p(ηm*) const
    { return φm(p(null<πsa<π1>>()), [](πsa<π1> &&f)
      { ηm b; πi i; f.x(i); b << i.ypop().all(); return b; }); }


protected:
  template<class T, class F> Γφ &def_(φ<T> &d, Stc &n, F &&f)
    { d.template as<φd_<T>>().def(n, φauto(self(), std::function(std::forward<F>(f))));
      return self(); }

  πφ    pf_;  // provider for π expressions

  φ<Γ>  e_;   // Γ-coerced expressions (incl Ψ)
  φ<Γ>  a_;   // Γ-coerced atoms (incl Ψ)
  φ<Γ>  g_;   // Γ atoms
  φ<Ψ0> p0_;  // Ψ0 atoms
  φ<Ψ1> p1_;  // Ψ1 atoms
  φ<Ψ2> p2_;  // Ψ2 atoms
  φ<Ψ4> p4_;  // Ψ4 atoms

  φ<Γ>  we_;  // wrap(φW(e_))
  φ<Γ>  wa_;
  φ<Γ>  wg_;
  φ<Ψ0> wp0_;
  φ<Ψ1> wp1_;
  φ<Ψ2> wp2_;
  φ<Ψ4> wp4_;

  φ<Γ>  wee_;  // φE(we_)
};


// Creates a Γ parser that accepts any of the specified Ψ atoms instead, setting
// their direction to what is specified. The resulting parser can also accept a
// group of like-typed Ψs encased in [].
φ<Γ> ΓφΨdir(Stc&, Ψd, φ<Ψ0>, φ<Ψ1>, φ<Ψ2>, φ<Ψ4>);


template<class Γφ, class πφ>
Γφ_<Γφ, πφ>::Γφ_()
  : e_ (φa0<Γ>("Γe")),
    a_ (φa0<Γ>("Γa")),
    g_ (φd<Γ> ("Γg")),
    p0_(φd<Ψ0>("Ψ0")),
    p1_(φd<Ψ1>("Ψ1")),
    p2_(φd<Ψ2>("Ψ2")),
    p4_(φd<Ψ4>("Ψ4")),

    we_ (φwrap(φW(e_))),
    wa_ (φwrap(φW(a_))),
    wg_ (φwrap(φW(g_))),
    wp0_(φwrap(φW(p0_))),
    wp1_(φwrap(φW(p1_))),
    wp2_(φwrap(φW(p2_))),
    wp4_(φwrap(φW(p4_))),
    wee_(φE(we_))
{
  e_.as<φa_<Γ>>() << φm(φn(wa_, 1), Γs);
  a_.as<φa_<Γ>>()
    << wg_
    << φgroup(we_)
    << ΓφΨdir("ΓΨ", Ψd::f, wp0_, wp1_, wp2_, wp4_)
    << φ2("Γ|",  φwrap(φl("|")),  ΓφΨdir("Γ|",  Ψd::r, wp0_, wp1_, wp2_, wp4_))
    << φ2("Γ\\", φwrap(φl("\\")), ΓφΨdir("Γ\\", Ψd::b, wp0_, wp1_, wp2_, wp4_));
}


template<class πφ, class... Xs>
struct Γφ : public Γφ_<Γφ<πφ, Xs...>, πφ>, φauto_str, Xs...
{
  using Γφ_<Γφ<πφ, Xs...>, πφ>::p;
  using φauto_str::p;
  using Xs::p...;
};


}

#include "end.hh"

#endif
