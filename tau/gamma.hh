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

  // TODO: add splices


  γ(γ &) = delete;
  γ(γ&&) = delete;
  γ(Φ &f_) : f(f_) {}
  ~γ() { φω(); for (let i : ls) if (i != f.l.i()) f.l.w(i); }


  γφ &operator[](φi i) const { return *fs.at(i); }

  λi  λc(λf &&f_) { let i = f.l.c(std::move(f_)); ls.insert(i); return i; }
  γ  &λx(λi i)    {         f.l.x(i);             ls.erase(i);  return *this; }
  Λr  λw(λi i)    {                               ls.erase(i);  return f.l.w(i); }
  γ  &λy()        {         f.l.y(λs::R);                       return *this; }

  γ &Θ(Θp t)  { f.Θ(t);          return *this; }
  γ &Θ(ΔΘ dt) { f.Θ(now() + dt); return *this; }


  φi φc(γφ *f, bool s = false)
    { for (φi i = 0; i < fs.size(); ++i)
        if (!fs[i]) { fs[i] = f; φs(i, s); return i; }
      fs.push_back(f);
      φs(fs.size() - 1, s);
      return fs.size() - 1; }

  φi φc(uf8 b = ζb0, bool s = false, bool nr = false, bool nw = false)
    { return φc(new γφ(f.l, b, nr, nw), s); }

  γ &φx(φi i)
    { if (fs[i])          { fs[i]->ω(); delete fs[i]; fs[i] = nullptr; }
      if (ss.contains(i)) { ss[i]->ω(); delete ss[i]; ss.erase(i); }
      return *this; }

  γ &φω() { for (uN i = 0; i < fs.size(); ++i) φx(i); return *this; }

  γφ *φsi(φi i) const { return ss.contains(i) ? ss.at(i) : nullptr; }
  γ   &φs(φi i, bool s)
    { if (s == ss.contains(i)) return *this;
      if (s) (*(ss[i] = new γφ(f.l, (*this)[i].b)))((*this)[i]);
      else { ss[i]->ω(); delete ss[i]; ss.erase(i); }
      return *this; }


  γ &operator()(φi i, γ &g, φi gi)
    { if (  φsi(i))  { let f = φc((*this)[i].b); *ss.at(i) << f; i = f; }
      if (g.φsi(gi)) { g(gi, *this, i); return *this; }
      (*this)[i](g[gi]);
      return *this; }
};


template<class T>
typename std::enable_if<iti<T>::v, bool>::type
operator>(T &x, γφ &y)
{
  for (let a : x) if (!(y << a)) return false;
  return true;
}


#if tau_debug_iostream
template<class T>
O &operator<<(O &s, φ9::φo<T> const &o)
{
  s << "φo ";
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
