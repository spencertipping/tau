#ifndef τΓφ_h
#define τΓφ_h

#include "phi.hh"
#include "phi-ctor.hh"
#include "Gamma.hh"
#include "Gamma-phi-basic.hh"
#include "Psi.hh"
#include "pi-phi.hh"

#include "begin.hh"

namespace τ
{


// An extensible Γ grammar. See pi-phi.hh for a similar construct for π.
template<class Γφ, class πφ = πφ0> struct Γφ_
{
  Γφ_();
  Γφ_(Γφ_ const&) = delete;
  Γφ_(Γφ_&&)      = delete;
  virtual ~Γφ_() {}

  Γφ       &self()       { return *Rc<Γφ*>(this); }
  Γφ const &self() const { return *Rc<Γφ const*>(this); }

  πφ       &pi()         { return pf_; }
  πφ const &pi()   const { return pf_; }


  Tt Γφ &def_g (Stc &n, T const &f) { return def_(g_,  n, f); }
  Tt Γφ &def_p0(Stc &n, T const &f) { return def_(p0_, n, f); }
  Tt Γφ &def_p1(Stc &n, T const &f) { return def_(p1_, n, f); }
  Tt Γφ &def_p2(Stc &n, T const &f) { return def_(p2_, n, f); }
  Tt Γφ &def_p4(Stc &n, T const &f) { return def_(p4_, n, f); }


  auto p(Γ*)  const { return we_; }
  auto p(Ψ0*) const { return wp0_; }
  auto p(Ψ1*) const { return wp1_; }
  auto p(Ψ2*) const { return wp2_; }
  auto p(Ψ4*) const { return wp4_; }

  Tt auto p(πsa<T> *x) const { return pf_.p(x); }
  Tt auto p(πpa<T> *x) const { return pf_.p(x); }
  Tt auto p(πse<T> *x) const { return pf_.p(x); }
  Tt auto p(πpe<T> *x) const { return pf_.p(x); }
  Tt auto p(πst<T> *x) const { return pf_.p(x); }
  Tt auto p(πpt<T> *x) const { return pf_.p(x); }


protected:
  template<class T, class F> Γφ &def_(φ<T> &d, Stc &n, F const &f)
    { d.template as<φd_<T>>().def(n, φauto(self(), std::function(f)));
      return self(); }

  πφ    pf_;  // provider for π expressions

  φ<Γ>  e_;   // Γ-coerced expressions
  φ<Γ>  a_;   // Γ-coerced atoms
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

    we_ (Γφwrap(φW(e_))),
    wa_ (Γφwrap(φW(a_))),
    wg_ (Γφwrap(φW(g_))),
    wp0_(Γφwrap(φW(p0_))),
    wp1_(Γφwrap(φW(p1_))),
    wp2_(Γφwrap(φW(p2_))),
    wp4_(Γφwrap(φW(p4_)))
{
  e_.as<φa_<Γ>>() << φm(φn(wa_, 1), Γs);
  a_.as<φa_<Γ>>()
    << wg_
    << Γφgroup(we_)
    << ΓφΨdir("ΓΨ", Ψd::f, wp0_, wp1_, wp2_, wp4_)
    << φ2("Γ|",  Γφwrap(φl("|")),  ΓφΨdir("Γ|",  Ψd::r, wp0_, wp1_, wp2_, wp4_))
    << φ2("Γ\\", Γφwrap(φl("\\")), ΓφΨdir("Γ\\", Ψd::b, wp0_, wp1_, wp2_, wp4_));
}


template<class... Xs>
struct Γφ : public Γφ_<Γφ<Xs...>>, φauto_str, Xs...
{
  using Γφ_<Γφ>::p;
  using φauto_str::p;
  using Xs::p...;
};


}

#include "end.hh"

#endif
