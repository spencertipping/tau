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

  i9 r(i9 x, ζ&)
    { ++im;
      free();
      if (x.type() == u9t::heapref) b = x = *x;
      return x; }

  template<class W> ic auto w(W x, ζ &z)
    { if constexpr (o9_<W>::v) { return os(x.size(), z.b.c) ? φo<decltype(x)>{o9box(x)} : φo<decltype(x)>{x}; }
      else      { let o = o9(x); return os(o.size(), z.b.c) ? φo<decltype(o)>{o9box(o)} : φo<decltype(o)>{o}; } }
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
    { fs.push_back(ιb ? new γφ(l, ιb) : nullptr);
      fs.push_back(οb ? new γφ(l, οb) : nullptr);
      fs.push_back(ξb ? new γφ(l, ξb) : nullptr);
      fs.push_back(δb ? new γφ(l, δb) : nullptr); }

  ~γ()
    { for (let i : ls) l.w(i);
      for (uN i = 0; i < fs.size(); ++i) φx(i); }


  γφ &operator[](φi i) const { return *fs.at(i); }

  γφ &ι() const { return *fs[0]; }
  γφ &ο() const { return *fs[1]; }
  γφ &ξ() const { return *fs[2]; }
  γφ &δ() const { return *fs[3]; }

  λi  λc(λf &&f) { let i = l.c(std::move(f)); ls.insert(i); return i; }
  γ  &λx(λi i)   {         l.x(i);            ls.erase(i);  return *this; }
  Λr  λw(λi i)   {                            ls.erase(i);  return l.w(i); }
  γ  &λy()       {         l.y(λs::R);                      return *this; }

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
