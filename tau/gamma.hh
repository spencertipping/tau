#ifndef tau_γ_h
#define tau_γ_h


#include "types.hh"
#include "Lambda.hh"
#include "phi.hh"
#include "utf9-i9.hh"
#include "utf9-o9.hh"


#include "begin.hh"

namespace tau
{


template<class F = φι>
struct γ
{
  Λ &l;

  S<λi>        ls;
  V<φ<i9, F>*> fs;

  γ(γ &) = delete;
  γ(Λ &l_, uf8 ιb = ζb0, uf8 οb = ζb0, uf8 ξb = ζb0, uf8 δb = ζb0)
    : l(l_)
    { fs.push_back(new φ<i9, F>(l, ιb));
      fs.push_back(new φ<i9, F>(l, οb));
      fs.push_back(new φ<i9, F>(l, ξb));
      fs.push_back(new φ<i9, F>(l, δb)); }

  ~γ()
    { for (let i : ls) l.x(i);
      for (uN i = 0; i < fs.size(); ++i) φx(i); }


  φ<i9, F> &operator[](φi i) const { return *fs.at(i); }

  φ<i9, F> &ι() const { return *fs[0]; }
  φ<i9, F> &ο() const { return *fs[1]; }
  φ<i9, F> &ξ() const { return *fs[2]; }
  φ<i9, F> &δ() const { return *fs[3]; }

  λi    λc(λf &&f) { let i = l.c(std::move(f)); ls.insert(i); return i; }
  γ<F> &λx(λi i)   { l.x(i); ls.erase(i); return *this; }
  Λr    λw(λi i)   { ls.erase(i); return l.w(i); }

  φi    φc(uf8 b = ζb0)
    { let f = new φ<i9, F>(l);
      for (φi i = 0; i < fs.size(); ++i)
        if (!fs[i]) { fs[i] = f; return i; }
      fs.push_back(f);
      return fs.size() - 1; }

  γ<F> &φx(φi i)
    { if (fs[i]) { fs[i]->ω(); delete fs[i]; fs[i] = nullptr; }
      return *this; }


  γ &operator|(γ &x) { ο()(x.ι()); return x; }
};


}

#include "end.hh"


#endif
