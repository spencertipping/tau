#ifndef τΞ_h
#define τΞ_h


#include "Lambda.hh"
#include "tau.hh"
#include "xiio.hh"

#include "begin.hh"

namespace τ
{


struct Γ_;


// Immutable stack of ξds
struct Ξs final
{
  ξd     const io;
  Sp<Ξs> const n;
};


// Immutable k → v map with scope-level annotations
template<class T>
struct Ξk final
{
  St     const k;
  T      const v;
  uN     const s;
  Sp<Ξk> const n;
};

typedef Ξk<ξd>     Ξkd;
typedef Ξk<Sp<Γ_>> ΞkG;


Tt T         Ξkg(Sp<Ξk<T>>, Stc&);                // get value for key
Tt Sp<Ξk<T>> Ξkc(Sp<Ξk<T>>, Stc&, T const&, uN);  // create k/v binding
Tt Sp<Ξk<T>> Ξks(Sp<Ξk<T>>, Stc&, T const&, uN);  // create or replace k/v binding
Tt bool      Ξki(Sp<Ξk<T>>, Stc&);                // check for existence
Tt Sp<Ξk<T>> Ξkx(Sp<Ξk<T>>, Stc&);                // delete k/v binding
Tt Sp<Ξk<T>> Ξkp(Sp<Ξk<T>>, uN);                  // pop scope


// Γ program state, including a stack of ξ duplexes
//
// NOTE: this doesn't need to be fast; it's just involved in pipeline setup.
// It's much more important that the API be simple and predictable.
struct Ξ final
{
  τe      &e_;  // environment
  Sp<Ξs>   t_;  // stack top
  Sp<Ξkd>  m_;  // keyed pairs
  Sp<Ξkd>  v_;  // variables
  Sp<ΞkG>  g_;  // bound Γs
  uN       s_;  // scope index (keyed things and vars will be ≤s)

  Ξ(τe &e)         : e_(e), t_(nullptr),            m_(nullptr), v_(nullptr), s_(0) {}
  Ξ(τe &e, ξdc &p) : e_(e), t_(new Ξs{p, nullptr}), m_(nullptr), v_(nullptr), s_(0) {}

  Ξ(τe &e, Sp<Ξs> t, Sp<Ξkd> m, Sp<Ξkd> v, Sp<ΞkG> g, uN s)
    : e_(e), t_(t), m_(m), v_(v), g_(g), s_(s) {}


  Ξ &operator=(Ξ const &x)
    { t_ = x.t_;
      m_ = x.m_;
      v_ = x.v_;
      g_ = x.g_;
      s_ = x.s_;
      return *this; }


  τe &t() const { return e_; }


  Ξ t(Sp<Ξs>  t) const { return {e_, t, m_, v_, g_, s_}; }
  Ξ m(Sp<Ξkd> m) const { return {e_, t_, m, v_, g_, s_}; }
  Ξ v(Sp<Ξkd> v) const { return {e_, t_, m_, v, g_, s_}; }
  Ξ g(Sp<ΞkG> g) const { return {e_, t_, m_, v_, g, s_}; }


  Sp<Γ_> gg(Stc &k)                  const { return Ξkg(g_, k); }
  Ξ      gs(Stc &k, Sp<Γ_> const &x) const { return g(Ξks(g_, k, x, s_)); }
  bool   gi(Stc &k)                  const { return Ξki(g_, k); }
  Ξ      gx(Stc &k)                  const { return g(Ξkx(g_, k)); }

  ξd   kg(Stc &k)         const { return Ξkg(m_, k); }
  Ξ    ks(Stc &k, ξdc &x) const { return m(Ξks(m_, k, x, s_)); }
  bool ki(Stc &k)         const { return Ξki(m_, k); }
  Ξ    kx(Stc &k)         const { return m(Ξkx(m_, k)); }

  ξd   vg(Stc &k)         const { return Ξkg(v_, k); }
  Ξ    vs(Stc &k, ξdc &x) const { return v(Ξks(v_, k, x, s_)); }
  Ξ    vc(Stc &k, ξdc &x) const { return v(Ξkc(v_, k, x, s_)); }
  bool vi(Stc &k)         const { return Ξki(v_, k); }
  Ξ    vx(Stc &k)         const { return v(Ξkx(v_, k)); }

  Ξ pushk(Stc &k) const { return push(kg(k)); }
  Ξ pushv(Stc &k) const { return push(vg(k)); }


  Ξ        spush() const { return {e_, t_, m_, v_, g_, s_ + 1}; }
  Ξ         spop() const { return {e_, t_, Ξkp(m_, s_ - 1), Ξkp(v_, s_ - 1), Ξkp(g_, s_ - 1), s_ - 1}; }
  M<St, ξd> ktop() const;
  M<St, ξd> vtop() const;


  ξi f()            const { return t_->io.f; }
  ξo b()            const { return t_->io.b; }
  Ξ  f(ξi const &x) const { let [io, t] = pop(); return t.push(ξd{x, io.b}); }
  Ξ  b(ξo const &x) const { let [io, t] = pop(); return t.push(ξd{io.f, x}); }
  Ξ  fx()           const { return f({}); }
  Ξ  bx()           const { return b({{}}); }

  bool fi() const { return t_ && t_->io.f; }
  bool bi() const { return t_ && t_->io.b; }


  Ξ    c (Stc &port) const;  // connect to server port
  bool ci(Stc &port) const { return Sc<bool>(e_.route(port)); }


  Ξ       push()       const { return t(Sp<Ξs>(new Ξs{ξdc{{}, {{}}}, t_})); }
  Ξ       push(ξdc &x) const { return t(Sp<Ξs>(new Ξs{x,             t_})); }
  Ξ       drop()       const { A(t_, "Ξ::drop() empty"); return t(t_->n); }
  P<ξd, Ξ> pop()       const { A(t_, "Ξ::pop() empty");  return mp(t_->io, t(t_->n)); }
  bool   empty()       const { return !t_; }
  uN     depth()       const
    { auto x = t_;
      uN   i = 0;
      while (x) ++i, x = x->n;
      return i; }
};


typedef Ξ const Ξc;


O &operator<<(O&, Ξc&);


}

#include "end.hh"


#endif
