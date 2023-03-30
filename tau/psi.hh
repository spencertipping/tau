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
  τe                   &t_;
  V<F<void(ψ_&)>>       df_;  // destructor functions
  S<λi>                 ls_;  // managed λ IDs
  St                    n_;   // name for debugging purposes
  M<St, F<Ξ(Ξ const&)>> cs_;  // connection functions

  ψ_(τe &t) : t_(t) {            ψc_(this); }
  ~ψ_()             { destroy(); ψx_(this); }

  τe &t() const { return t_; }

  ψ_ &f (λf&&);
  ψ_ &fx(F<void(ψ_&)> &&f) { df_.push_back(std::move(f)); return *this; }

  Ξ connect(Stc &port, Ξ const &x)
    { A(cs_.contains(port), n_ << " does not bind " << port);
      return cs_[port](x); }

  void destroy();
};


// Public interface to ψ_
struct ψ final
{
  ψ() {}
  ψ(τe    &t) : q_(new ψ_(t)) {}
  ψ(Sp<ψ_> q) : q_(q) {}
  ψ(ψ_    *q) : q_(q) {}

  ψ(τe &t, St n) : ψ(t) { name(n); }

  ψ &operator=(ψ const &x) { q_ = x.q_; return *this; }

  Stc &name() const {             return q_->n_; }
  ψ   &name(Stc &n) { q_->n_ = n; return *this; }

  ψ &f (λf           &&f) { q_->f (std::move(f)); return *this; }
  ψ &fx(F<void(ψ_&)> &&f) { q_->fx(std::move(f)); return *this; }
  ψ &f (F<void(ψ&)>  &&f) { return this->f([f=std::move(f), this]() { f(*this); }); }

  ψ &b (Stc &p, F<Ξ(ψ&&, Ξc&)> &&f)
    { A(!q_->t().route(p), name() << " cannot bind claimed port " << p);
      q_->cs_[p] = [f=std::move(f), q=q_](Ξc &x) { return f(ψ(q), x); };
      q_->t().bind(p, q_);
      return *this; }

  ψ &bx(Stc &p)
    { A(q_->cs_.contains(p), name() << " cannot unbind " << p);
      q_->t().unbind(p);
      q_->cs_.erase(p);
      return *this; }

  ψ &i(ξi const &i) { if (let x = i.inner_ξ()) x->oq(q_); return *this; }
  ψ &o(ξo const &o) { if (let x = o.inner_ξ()) x->iq(q_); return *this; }


protected:
  Sp<ψ_> q_;
};


O &operator<<(O&, ψ  const&);
O &operator<<(O&, ψ_ const&);


}

#include "end.hh"


#endif
