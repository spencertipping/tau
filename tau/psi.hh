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


struct ψ;

void ψc_(ψ*);  // notify that a new ψ has been created
void ψx_(ψ*);  // notify that a ψ has been destroyed
uN   ψn();     // number of live ψs


// Extensible ψ process
struct ψ
{
  template<class... Xs>
  ψ(τe &t__, Xs&&... xs) : t_(t__) { def(xs...); ψc_(this); }

  virtual ~ψ() { destroy(); ψx_(this); }

  virtual St name() const {         return n_; }
  virtual ψ &name(Stc &s) { n_ = s; return *this; }

  virtual Ξ connect(Stc &port, Ξ const &x)
    { A(0, "ψ::connect(" << port << ") on non-server " << name() << "; Ξ = " << x);
      τunreachable(); }

  τe &t() const { return t_; }


  ψ &def() { return *this; }
  template<class... Xs> ψ &def(λf          &&f, Xs&&... xs) { ls_.insert(λc(std::move(f))); return def(xs...); }
  template<class... Xs> ψ &def(F<void(ψ&)> &&f, Xs&&... xs) { return def([f=std::move(f), this]() { f(*this); }, xs...); }


  // Creates a new λ and clears any unused λis from our tracked set.
  λi λc(λf&&);

  ψ &xf(F<void(ψ&)> &&f) { df_.push_back(std::move(f)); return *this; }


protected:
  τe                       &t_;
  V<F<void(ψ&)>>            df_;  // destructor functions
  S<λi>                     ls_;  // managed λ IDs
  St                        n_;   // name for debugging purposes
  M<St, F<Ξ(ψ&, Ξ const&)>> cs_;  // connection functions

  virtual void destroy();
};


O &operator<<(O&, ψ const&);


}

#include "end.hh"


#endif
