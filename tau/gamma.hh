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


struct φ9
{
  u64 im  = 0;
  u64 om  = 0;
  u64 ohc = 0;

  ic void s(uN s, uN zs)
    { if (s >= zs >> 1) ohc++;
      om++; }

  template<class R> ic R r(R x, ζ&) { ++im; return x; }

  template<class W> ic auto w(W x, ζ& z)
    { if constexpr (o9_<W>::v) { s(x.size(), z.b.c); return x; }
      else
      { let o = o9(x);
        s(o.size(), z.b.c);
        return o; } }
};


typedef φ<i9, φ9> γφ;


struct γ
{
  Λ &l;

  S<λi>  ls;
  V<γφ*> fs;

  γ(γ &) = delete;
  γ(Λ &l_, uf8 ιb = ζb0, uf8 οb = ζb0, uf8 ξb = ζb0, uf8 δb = ζb0)
    : l(l_)
    { fs.push_back(new γφ(l, ιb));
      fs.push_back(new γφ(l, οb));
      fs.push_back(new γφ(l, ξb));
      fs.push_back(new γφ(l, δb)); }

  ~γ()
    { for (let i : ls) l.w(i);
      for (uN i = 0; i < fs.size(); ++i) φx(i); }


  γφ &operator[](φi i) const { return *fs.at(i); }

  γφ &ι() const { return *fs[0]; }
  γφ &ο() const { return *fs[1]; }
  γφ &ξ() const { return *fs[2]; }
  γφ &δ() const { return *fs[3]; }

  λi  λc(λf &&f) { let i = l.c(std::move(f)); ls.insert(i); return i; }
  γ  &λx(λi i)   { l.x(i); ls.erase(i); return *this; }
  Λr  λw(λi i)   { ls.erase(i); return l.w(i); }

  φi φc(uf8 b = ζb0)
    { let f = new γφ(l);
      for (φi i = 0; i < fs.size(); ++i)
        if (!fs[i]) { fs[i] = f; return i; }
      fs.push_back(f);
      return fs.size() - 1; }

  γ &φx(φi i)
    { if (fs[i]) { fs[i]->ω(); delete fs[i]; fs[i] = nullptr; }
      return *this; }


  γ &operator^(γ &x) { ξ()(x.ο()); return x; }
  γ &operator|(γ &x) { ο()(x.ι()); return x; }
  γ &operator&(γ &x) { δ()(x.ι()); return x; }
};


}

#include "end.hh"


#endif
