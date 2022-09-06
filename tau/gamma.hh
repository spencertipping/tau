#ifndef tau_γ_h
#define tau_γ_h


#include "types.hh"
#include "Lambda.hh"
#include "phi.hh"
#include "Phi.hh"
#include "utf9-i9.hh"
#include "utf9-o9.hh"
#include "phi9.hh"


#include "begin.hh"

namespace tau
{


typedef φ<i9, i9, φ9> γφ;


struct γ
{
  Φ &f;

  S<λi>  ls;
  V<γφ*> fs;

  γ(γ &) = delete;
  γ(Φ &f_, uf8 ιb = ζb0, uf8 οb = ζb0, uf8 δb = ζb0, uf8 εb = ζb0)
    : f(f_)
    { fs.push_back(ιb ? new γφ(f.l, ιb) : nullptr);
      fs.push_back(οb ? new γφ(f.l, οb) : nullptr);
      fs.push_back(δb ? new γφ(f.l, δb) : nullptr);
      fs.push_back(εb ? new γφ(f.l, εb) : nullptr); }

  // TODO: in the future, no destructor required; γ should manage its own
  // lifecycle
  ~γ()
    { for (let i : ls) f.l.w(i);
      for (uN i = 0; i < fs.size(); ++i) φx(i); }


  γφ &operator[](φi i) const { return *fs.at(i); }

  γφ &ι() const { return *fs[0]; }
  γφ &ο() const { return *fs[1]; }
  γφ &δ() const { return *fs[2]; }
  γφ &ε() const { return *fs[3]; }

  λi  λc(λf &&f_) { let i = f.l.c(std::move(f_)); ls.insert(i); return i; }
  γ  &λx(λi i)    {         f.l.x(i);             ls.erase(i);  return *this; }
  Λr  λw(λi i)    {                               ls.erase(i);  return f.l.w(i); }
  γ  &λy()        {         f.l.y(λs::R);                       return *this; }

  φi φc(uf8 b = ζb0)
    { let x = new γφ(f.l);
      for (φi i = 0; i < fs.size(); ++i)
        if (!fs[i]) { fs[i] = x; return i; }
      fs.push_back(x);
      return fs.size() - 1; }

  γ &φx(φi i)
    { if (fs[i]) { fs[i]->ω(); delete fs[i]; fs[i] = nullptr; }
      return *this; }

  γ &Θ(Θp t)  { f.Θ(t);          return *this; }
  γ &Θ(ΔΘ dt) { f.Θ(now() + dt); return *this; }


  template<class T>
  bool operator<<(T x) { return ο() << x; }


  γ &operator^(γ &x) { δ()(x.ο()); return x; }
  γ &operator|(γ &x) { ο()(x.ι()); return x; }
  γ &operator&(γ &x) { ε()(x.ι()); return x; }
};


#if tau_debug_iostream
template<class T>
O &operator<<(O &s, φ9::φo<T> const &o)
{
  return s << "φo ";
  if (o.x.index() == 0) s << std::get<0>(o.x);
  else                  s << std::get<1>(o.x);
  return s;
}

O &operator<<(O &s, φ9 const &f)
{
  return s << "φ9[" << f.im << " " << f.om << "+" << f.ohc
           << "^" << f.ov << (f.b ? " H" : "") << "]";
}

O &operator<<(O &s, γ const &g)
{
  s << "γ λs=";
  for (let i : g.ls) s << i << " ";
  s << std::endl;

  for (uN i = 0; i < g.fs.size(); ++i)
    if (let f = g.fs[i])
      s << "  " << i << " " << f->f << " " << *f << std::endl;
  return s;
}
#endif


}

#include "end.hh"


#endif
