#ifndef τΞ_h
#define τΞ_h


#include "Lambda.hh"
#include "tau.hh"
#include "xiio.hh"

#include "begin.hh"

namespace τ
{


// Immutable stack of ξds
struct Ξs final
{
  ξd     const io;
  Sp<Ξs> const n;
};


// Immutable k → v map with scope-level annotations
struct Ξk final
{
  St     const k;
  ξd     const v;
  uN     const s;
  Sp<Ξk> const n;
};


ξd     Ξkg(Sp<Ξk>, Stc&);                 // get value for key
Sp<Ξk> Ξkc(Sp<Ξk>, Stc&, ξd const&, uN);  // create k/v binding
Sp<Ξk> Ξks(Sp<Ξk>, Stc&, ξd const&, uN);  // create or replace k/v binding
bool   Ξki(Sp<Ξk>, Stc&);                 // check for existence
Sp<Ξk> Ξkx(Sp<Ξk>, Stc&);                 // delete k/v binding
Sp<Ξk> Ξkp(Sp<Ξk>, uN);                   // pop scope


// Γ program state, including a stack of ξ duplexes
//
// NOTE: this doesn't need to be fast; it's just involved in pipeline setup.
// It's much more important that the API be simple and predictable.
struct Ξ final
{
  τe     &e_;  // environment
  Sp<Ξs>  t_;  // stack top
  Sp<Ξk>  m_;  // keyed pairs
  Sp<Ξk>  v_;  // variables
  uN      s_;  // scope index (keyed things and vars will be ≤s)

  Ξ(τe &e)              : e_(e), t_(nullptr),            m_(nullptr), v_(nullptr), s_(0) {}
  Ξ(τe &e, ξd const &p) : e_(e), t_(new Ξs{p, nullptr}), m_(nullptr), v_(nullptr), s_(0) {}
  Ξ(τe &e, Sp<Ξs> t, Sp<Ξk> m, Sp<Ξk> v, uN s) : e_(e), t_(t), m_(m), v_(v), s_(s) {}


  Ξ &operator=(Ξ const &x)
    { t_ = x.t_;
      m_ = x.m_;
      v_ = x.v_;
      s_ = x.s_;
      return *this; }


  Ξ t(Sp<Ξs> t) const { return {e_, t, m_, v_, s_}; }
  Ξ m(Sp<Ξk> m) const { return {e_, t_, m, v_, s_}; }
  Ξ v(Sp<Ξk> v) const { return {e_, t_, m_, v, s_}; }


  ξd   kg(Stc &k)              const { return Ξkg(m_, k); }
  Ξ    ks(Stc &k, ξd const &x) const { return m(Ξks(m_, k, x, s_)); }
  bool ki(Stc &k)              const { return Ξki(m_, k); }
  Ξ    kx(Stc &k)              const { return m(Ξkx(m_, k)); }

  ξd   vg(Stc &k)              const { return Ξkg(v_, k); }
  Ξ    vs(Stc &k, ξd const &x) const { return v(Ξks(v_, k, x, s_)); }
  Ξ    vc(Stc &k, ξd const &x) const { return v(Ξkc(v_, k, x, s_)); }
  bool vi(Stc &k)              const { return Ξki(v_, k); }
  Ξ    vx(Stc &k)              const { return v(Ξkx(v_, k)); }

  Ξ pushk(Stc &k) const { return push(kg(k)); }
  Ξ pushv(Stc &k) const { return push(vg(k)); }


  Ξ vpush() const { return {e_, t_, m_, v_, s_ + 1}; }
  Ξ  vpop() const { return {e_, t_, Ξkp(m_, s_ - 1), Ξkp(v_, s_ - 1), s_ - 1}; }


  Ξ f(ξi const &x) const { let [io, t] = pop(); return t.push(ξd{x, io.b}); }
  Ξ b(ξo const &x) const { let [io, t] = pop(); return t.push(ξd{io.f, x}); }

  bool fi() const { return t_ && t_->io.f; }
  bool bi() const { return t_ && t_->io.b; }


  Ξ    c (Stc &port) const;  // connect to server port
  bool ci(Stc &port) const { return Sc<bool>(e_.route(port)); }


  Ξ       push(ξd const &x) const { return t(Sp<Ξs>(new Ξs{x, t_})); }
  Ξ       drop()            const { A(t_, "Ξ::drop() empty"); return t(t_->n); }
  P<ξd, Ξ> pop()            const { A(t_, "Ξ::pop() empty");  return mp(t_->io, t(t_->n)); }
  bool   empty()            const { return !t_; }
  uN     depth()            const
    { auto x = t_;
      uN   i = 0;
      while (x) ++i, x = x->n;
      return i; }
};


O &operator<<(O&, Ξ const&);


}

#include "end.hh"


#endif
