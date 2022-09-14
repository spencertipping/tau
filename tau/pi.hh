#ifndef τπ_h
#define τπ_h


#include "types.hh"
#include "numerics.hh"
#include "Phi.hh"
#include "Lambda.hh"
#include "zeta.hh"
#include "utf9.hh"


#include "begin.hh"

namespace τ
{


typedef uN πr;


struct πh
{
  Λ     &l;
  ζ<i9> *h;
  V<uN>  d;  // data registers: offsets into *h
  u64    a;  // total allocations

  πh(Λ &l_) : l(l_), h(new ζ<i9>(l, ζb0)), a(0) {}


  uN c()  const { return h->b.c; }
  uN wa() const { return h->b.wa(); }
  uN ra() const { return h->b.ra(); }


  uN hs(uN l) const  // live set size → new heap size
    { return l * 2 > c() ? l * 2
           : l * 8 < c() ? std::max(Sc<uN>(1) << ζb0, c() >> 2)
           :               c(); }


  πh &gc(uN s)
    { uN n = s;  // total live-set size after this allocation
      for (let x : d) n += i9::size_of(*h + x);
      let oh = h;
      h = new ζ<i9>(l, ilog(hs(n)) + 1);
      for (auto &x : d) x = *this << i9{*oh + x};
      delete oh;
      return *this; }

  uN operator<<(i9 v)
    { let s = i9::size_of(v); if (s > wa()) gc(s);
      let r = h->b.wi;        A(*h << o9(v), "GC internal error");
      a += s;
      return r; }

  πh &operator()(πr k, i9 v) { d[k] = *this << v; return *this; }
  i9  operator[](πr k) const { return i9{*h + d[k]}; }
};


struct πi
{
  Φ      &f;
  πh      h;
  Sk<ζp>  r;   // return stack

  πi(Φ &f_, ζp r_) : f(f_), h(f.l) { r.push(r_); }

  operator bool() const { return !r.empty(); }
};


typedef uN         πr;  // zero for success
typedef F<πr(πi&)> πF;





}

#include "end.hh"


#endif
