#ifndef tau_γ_h
#define tau_γ_h


#include "types.hh"
#include "Lambda.hh"
#include "phi.hh"
#include "Phi.hh"
#include "utf9-i9.hh"
#include "utf9-o9.hh"


#include "begin.hh"

namespace tau
{


struct φ9
{
  template<class T>
  struct φo
  {
    Va<T, o9f<u9_heapref>> x;

    uN size ()     { return x.index() ? std::get<1>(x).size()   : std::get<0>(x).size(); }
    uN write(ζp m) { return x.index() ? std::get<1>(x).write(m) : std::get<0>(x).write(m); }
  };


  u64 im  = 0;  // total inbound messages
  u64 om  = 0;  // total outbound messages
  u64 ohc = 0;  // messages over half-ζ-capacity
  u64 ov  = 0;  // messages that overflowed (and were boxed)

  // Most recently-received heap-allocated message, which remains live
  // until (1) it is taken (then you free it manually), (2) you read
  // another message, (3) you destroy this φ9, or (4) you explicitly
  // call .free().
  ζp b = nullptr;

  ~φ9() { free(); }


  ic bool os(uN s, uN zs)
    { ++om;
      if (s >= zs >> 1) ++ohc;
      let v = s >= zs - 1;
      if (v) ++ov;
      return v; }


  i9   take() { let r = b; b = nullptr; return r; }
  void free() { if (b) { std::free(b); b = nullptr; } }

  template<class R>
  i9 r(i9 x, ζ<R>&)
    { ++im;
      free();
      if (x.type() == u9t::heapref) b = x = *x;
      return x; }

  template<class W, class Z> ic auto w(W x, ζ<Z> &z)
    { if constexpr (o9_<W>::v) { return os(x.size(), z.b.c) ? φo<decltype(x)>{o9box(x)} : φo<decltype(x)>{x}; }
      else      { let o = o9(x); return os(o.size(), z.b.c) ? φo<decltype(o)>{o9box(o)} : φo<decltype(o)>{o}; } }
};


typedef φ<i9, i9, φ9> γφ;


struct γ
{
  Φ &f;

  S<λi>  ls;
  V<γφ*> fs;

  γ(γ &) = delete;
  γ(Φ &f_, uf8 ιb = ζb0, uf8 οb = ζb0, uf8 ξb = ζb0, uf8 δb = ζb0)
    : f(f_)
    { fs.push_back(ιb ? new γφ(f.l, ιb) : nullptr);
      fs.push_back(οb ? new γφ(f.l, οb) : nullptr);
      fs.push_back(ξb ? new γφ(f.l, ξb) : nullptr);
      fs.push_back(δb ? new γφ(f.l, δb) : nullptr); }

  ~γ()
    { for (let i : ls) f.l.w(i);
      for (uN i = 0; i < fs.size(); ++i) φx(i); }


  γφ &operator[](φi i) const { return *fs.at(i); }

  γφ &ι() const { return *fs[0]; }
  γφ &ο() const { return *fs[1]; }
  γφ &ξ() const { return *fs[2]; }
  γφ &δ() const { return *fs[3]; }

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


  γ &operator^(γ &x) { ξ()(x.ο()); return x; }
  γ &operator|(γ &x) { ο()(x.ι()); return x; }
  γ &operator&(γ &x) { δ()(x.ι()); return x; }
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
