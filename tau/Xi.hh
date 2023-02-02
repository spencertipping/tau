#ifndef τΞ_h
#define τΞ_h


#include "Lambda.hh"
#include "tau.hh"
#include "psi.hh"
#include "xiio.hh"

#include "begin.hh"

namespace τ
{


// Default size of ξs created with Ξ
letc Ξc0 = 8192;


// A mutable cable of full-duplex ξ connections
struct Ξ
{
  Ξ(τe &t) : t_(t), p_(new ξd(l())) {}
  Ξ(Ξ&&)      = delete;
  Ξ(Ξ const&) = delete;

  Λ  &l() const { return t_.l(); }
  τe &t() const { return t_; }

  ξd            &p() { return *p_; }
  V<Sp<ξd>>     &a() { return  a_; }
  M<St, Sp<ξd>> &n() { return  n_; }
  M<St, Sp<Ξ>>  &x() { return  x_; }


  P<ξi, ξo> xf(Sp<ψ> q, uN cl = Ξc0, uN cr = Ξc0) { return p_->xf(q, cl, cr); }
  P<ξo, ξi> xb(Sp<ψ> q, uN cl = Ξc0, uN cr = Ξc0) { return p_->xb(q, cl, cr); }

  ξd &operator()(Stc &k)
    { if (!n_.contains(k)) n_[k].reset(new ξd(l()));
      return *n_.at(k); }

  Ξ &operator[](Stc &k)
    { if (!x_.contains(k)) x_[k].reset(new Ξ(t_));
      return *x_.at(k); }

  Ξ &operator<<(Sp<ξd> x) { a_.push_back(x); return *this; }

  V<Sp<ξd>>::iterator begin() { return a_.begin(); }
  V<Sp<ξd>>::iterator end()   { return a_.end(); }


protected:
  τe           &t_;
  Sp<ξd>        p_;  // primary duplex
  V<Sp<ξd>>     a_;  // anonymous duplex bundle
  M<St, Sp<ξd>> n_;  // named duplexes
  M<St, Sp<Ξ>>  x_;  // named bundles

  friend O &operator<<(O&, Ξ const&);
};


O &operator<<(O&, Ξ const&);


}

#include "end.hh"


#endif
