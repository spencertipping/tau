#ifndef τψ_h
#define τψ_h


#include "lambda-types.hh"
#include "Lambda.hh"
#include "tau.hh"
#include "xiio.hh"
#include "begin.hh"

namespace τ
{


struct ψ;

void ψc_(ψ*);  // notify that a new ψ has been created
void ψx_(ψ*);  // notify that a ψ has been destroyed
uN ψn();       // number of live ψs


// Virtual base for ψ processes
struct ψ
{
  template<class... Xs>
  ψ(τe &t__, Xs... xs) : t_(t__) { def(xs...); ψc_(this); }

  virtual ~ψ();

  virtual St name() const {         return n_; }
  virtual ψ &name(Stc &s) { n_ = s; return *this; }

  τe &t() const { return t_; }


  // Resource definitions: can be functions, which are λs, or ξi/ξo,
  // which will be named.
  ψ &def() { return *this; }

  template<class... Xs> ψ &def(λf           &&f, Xs... xs) { ls.insert(λc(std::move(f))); return def(xs...); }
  template<class... Xs> ψ &def(F<void(ψ&)>  &&f, Xs... xs) { return def([f=std::move(f), this]() { f(*this); }, xs...); }
  template<class... Xs> ψ &def(Stc &k, ξi i,     Xs... xs) { i_[k] = i; return def(xs...); }
  template<class... Xs> ψ &def(Stc &k, ξo o,     Xs... xs) { o_[k] = o; return def(xs...); }


  // Creates a new λ and clears any unused λis from our tracked set.
  λi λc(λf&&);


  ξi i(Stc &k) const { return i_.at(k); }
  ξo o(Stc &k) const { return o_.at(k); }


  ψ &xf(F<void(ψ&)> &&f) { df.push_back(std::move(f)); return *this; }


protected:
  τe            &t_;
  V<F<void(ψ&)>> df;  // destructor functions
  S<λi>          ls;  // managed λ IDs
  M<St, ξi>      i_;  // named input ξs
  M<St, ξo>      o_;  // named output ξs
  St             n_;  // name for debugging purposes
};


O &operator<<(O&, ψ const&);


}

#include "end.hh"


#endif
