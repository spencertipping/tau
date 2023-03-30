#ifndef τψ_h
#define τψ_h


#include "lambda-types.hh"
#include "Lambda.hh"
#include "tau.hh"
#include "xiio.hh"
#include "Xi.hh"
#include "begin.hh"

namespace τ
{


struct ψ_;

void ψc_(ψ_*);  // notify that a new ψ_ has been created
void ψx_(ψ_*);  // notify that a ψ_ has been destroyed
uN   ψn ();     // number of live ψs

#if τtrack_ψ
S<ψ_*> &ψs();   // return set of live ψs
#endif


// ψ resource container; not used directly, use ψ instead
struct ψ_ final
{
  τe              &t_;
  V<F<void(ψ_&)>>  df_;  // destructor functions
  S<λi>            ls_;  // managed λ IDs
  St               n_;   // name for debugging purposes
  M<St, F<Ξ(Ξc&)>> cs_;  // connection functions

  ψ_(τe &t) : t_(t) {            ψc_(this); }
  ~ψ_()             { destroy(); ψx_(this); }

  τe &t() const { return t_; }

  ψ_ &f (λf&&);
  ψ_ &fx(F<void(ψ_&)> &&f) { df_.push_back(std::move(f)); return *this; }

  Ξ connect(Stc &port, Ξc &x)
    { A(cs_.contains(port), n_ << " does not bind " << port);
      return cs_[port](x); }

  void destroy();
};


// Public interface to ψ_
struct ψ final
{
  ψ(τe    &t) : t_(t), q_(new ψ_(t)) {}
  ψ(Sp<ψ_> q) : t_(q->t_), q_(q) {}
  ψ(ψ_    *q) : t_(q->t_), q_(q) {}

  ψ(τe &t, St n) : ψ(t) { name(n); }

  ψ &operator=(ψ const &x) { q_ = x.q_; w_ = x.w_; return *this; }
  operator bool() const { return q_ || w_; }

  Stc &name() const {              return q()->n_; }
  ψ   &name(Stc &n) { q()->n_ = n; return *this; }

  ψ &f (λf           &&f) { let r = q(); A(r, "ψ::f() on null");  r->f (std::move(f)); return *this; }
  ψ &fx(F<void(ψ_&)> &&f) { let r = q(); A(r, "ψ::fx() on null"); r->fx(std::move(f)); return *this; }
  ψ &f (F<void(ψ&)>  &&f) { return this->f([f=std::move(f), this]() { f(*this); }); }

  ψ &b (Stc &p, F<Ξ(ψ&&, Ξc&)> &&f)
    { let r = q();
      A(r, "ψ::b(" << p << ") on null ψ");
      A(!r->t().route(p), name() << " cannot bind claimed port " << p);
      r->cs_[p] = [f=std::move(f), q=r](Ξc &x) { return f(ψ(q), x); };
      r->t().bind(p, r);
      return *this; }

  ψ &bx(Stc &p)
    { let r = q(); if (!r) return *this;
      A(r->cs_.contains(p), name() << " cannot unbind " << p);
      r->t().unbind(p);
      r->cs_.erase(p);
      return *this; }

  ψ &i(ξi const &i) { if (let r = q()) if (let x = i.inner_ξ()) x->oq(r); return *this; }
  ψ &o(ξo const &o) { if (let r = q()) if (let x = o.inner_ξ()) x->iq(r); return *this; }

  Sp<ψ_> q() const { return q_ ? q_ : w_.lock(); }
  Wp<ψ_> w() const { return q_ ? q_ : w_; }

  // Subtle semantics: weakening will either silently weaken the ψ_ reference,
  // or it will terminate the λ thread if the ψ_ is freed (if bail is true).
  //
  // As a rule: call weaken(true) when you're weakening from inside a λ that
  // belongs to this ψ.
  ψ &weaken(bool bail = false)
    { if (!weaken_() && bail) t_.l().x(t_.l().i());
      return *this; }

  // Returns true if the ψ is still alive after weakening.
  bool weaken_() { if (q_) w_ = q_, q_.reset(); return !w_.expired(); }


protected:
  τe    &t_;
  Sp<ψ_> q_;
  Wp<ψ_> w_;
};


O &operator<<(O&, ψ  const&);
O &operator<<(O&, ψ_ const&);


}

#include "end.hh"


#endif
