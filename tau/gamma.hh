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
  Φ          &f;
  S<λi>       ls;
  V<γφ*>      fs;
  M<φi, γφ*>  ss;  // server sockets (used internally)


  γ(γ &) = delete;
  γ(Φ &f_, uf8 αb = ζb0, uf8 βb = ζb0, uf8 δb = ζb0, uf8 εb = ζb0)
    : f(f_)
    { fs.push_back(αb ? new γφ(f.l, αb) : nullptr);
      fs.push_back(βb ? new γφ(f.l, βb) : nullptr);
      fs.push_back(δb ? new γφ(f.l, δb) : nullptr);
      fs.push_back(εb ? new γφ(f.l, εb) : nullptr); }

  ~γ() { ω(); for (let i : ls) f.l.w(i); }


  γ &ω()
    { for (uN i = 0; i < fs.size(); ++i) φx(i);
      return *this; }


  γφ &operator[](φi i) const { return *fs.at(i); }

  γφ &α() const { return (*this)[0]; }
  γφ &β() const { return (*this)[1]; }
  γφ &δ() const { return (*this)[2]; }
  γφ &ε() const { return (*this)[3]; }

  λi  λc(λf &&f_) { let i = f.l.c(std::move(f_)); ls.insert(i); return i; }
  γ  &λx(λi i)    {         f.l.x(i);             ls.erase(i);  return *this; }
  Λr  λw(λi i)    {                               ls.erase(i);  return f.l.w(i); }
  γ  &λy()        {         f.l.y(λs::R);                       return *this; }

  γ &Θ(Θp t)  { f.Θ(t);          return *this; }
  γ &Θ(ΔΘ dt) { f.Θ(now() + dt); return *this; }


  φi φc(uf8 b = ζb0, bool s = false)
    { let x = new γφ(f.l, b);
      for (φi i = 0; i < fs.size(); ++i)
        if (!fs[i]) { fs[i] = x; return i; }
      fs.push_back(x);
      φs(fs.size() - 1, s);
      return fs.size() - 1; }

  γ &φx(φi i)
    { if (fs[i]) { fs[i]->ω(); delete fs[i]; fs[i] = nullptr; }
      if (ss.contains(i)) { delete ss[i]; ss.erase(i); }
      return *this; }

  γφ *φsi(φi i) const { return ss.contains(i) ? ss.at(i) : nullptr; }
  γ   &φs(φi i, bool s)
    { if (s == ss.contains(i)) return *this;
      if (s) (*(ss[i] = new γφ(f.l, (*this)[i].b)))((*this)[i]);
      else { ss[i]->ω(); delete ss[i]; ss.erase(i); }
      return *this; }


  γ &operator()(φi i, γ &g, φi gi)
    { if (φsi(i)) { let f = φc((*this)[i].b); (*this)[i] << f; g(gi, *this, f); return *this; }
      (*this)[i](g[gi]);
      return *this; }


  template<class T>
  bool operator<<(T x) { return β() << x; }

  γφ::it begin() const { return α().begin(); }
  γφ::it end()   const { return α().end(); }

  γ &operator^(γ &x) { δ()(x.β()); return x; }
  γ &operator|(γ &x) { β()(x.α()); return x; }
  γ &operator&(γ &x) { ε()(x.α()); return x; }
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
      s << "  " << i << (g.φsi(i) ? "S" : "-") << " "
        << f->f << " " << *f << std::endl;
  return s;
}
#endif


}

#include "end.hh"


#endif
