#ifndef τψ_h
#define τψ_h


#include "lambda-types.hh"
#include "Lambda.hh"
#include "xiio.hh"
#include "begin.hh"

namespace τ
{


// Virtual base for ψ processes
struct ψ
{
  template<class... Xs>
  ψ(Λ &l_, Xs... xs) : l(l_) { def(xs...); }

  virtual ~ψ() { for (let x : ls) if (l.e(x) && x != l.i()) l.x(x); }

  virtual Stc &name() const {         return n_; }
  virtual ψ   &name(Stc &s) { n_ = s; return *this; }


  // Resource definitions: can be functions, which are λs, or ξi/ξo,
  // which will be named.
  ψ &def() { return *this; }

  template<class... Xs> ψ &def(λf           &&f, Xs... xs) { ls.insert(λc(std::move(f))); return def(xs...); }
  template<class... Xs> ψ &def(F<void(ψ&)>  &&f, Xs... xs) { return def([f=std::move(f), this]() { f(*this); }, xs...); }
  template<class... Xs> ψ &def(Stc &k, ξi i,     Xs... xs) { i_[k] = i; return def(xs...); }
  template<class... Xs> ψ &def(Stc &k, ξo o,     Xs... xs) { o_[k] = o; return def(xs...); }


  // Creates a new λ and clears any unused λis from our tracked set.
  λi λc(λf &&f)
    { for (auto i = ls.begin(); i != ls.end();)
        if (!l.e(*i)) i = ls.erase(i); else ++i;
      return l.c(std::move(f)); }


  ξi i(Stc &k) const { return i_.at(k); }
  ξo o(Stc &k) const { return o_.at(k); }


protected:
  Λ        &l;
  S<λi>     ls;
  M<St, ξi> i_;  // named input ξs
  M<St, ξo> o_;  // named output ξs
  St        n_;  // name for debugging purposes
};


}

#include "end.hh"


#endif
