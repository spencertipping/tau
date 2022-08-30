#ifndef tau_φ_h
#define tau_φ_h


#include "gate.hh"
#include "types.hh"
#include "zeta.hh"
#include "pi.hh"
#include "shd.hh"

#include "begin.hh"

namespace tau
{


struct φι  // identity filter
{
  template<class R> ic R r(R x, ζ&) { return x; }
  template<class W> ic W w(W x, ζ&) { return x; }
};


struct φc  // capacity check filter
{
  u64 om  = 0;
  u64 ohc = 0;

  template<class R> ic R r(R x, ζ&) { return x; }
  template<class W> ic W w(W x, ζ& z)
    { if (x.size() >= z.b.c >> 1) ohc++;
      om++;
      return x; }
};


struct φπ  // profiling+timing filter
{
  ΣΘΔ i,  o;
  πι  is, os;
  φπ() { i.start(); o.start(); }
  template<class R> R r(R x, ζ&) { i.stop(); i.start(); is << x.size(); return x; }
  template<class W> W w(W x, ζ&) { o.stop(); o.start(); os << x.size(); return x; }
};


template<class R, class F = φι>
struct φ
{
  Λ       &l;
  φ<R, F> *c {nullptr};  // NOTE: template args may vary; ptr type is a lie
  uf8c     b;
  ζ       *i {nullptr};
  ζ       *o {nullptr};
  λg       cg;
  λg       xg;
  F        f;


  φ(φ &) = delete;
  φ(Λ &l_, uf8 b = ζb0) : l{l_}, b{b}, cg{l}, xg{l} {}
  ~φ()
    { rx();
      if (c) c->c = nullptr; }


  φ &operator()(ζ &i_, ζ &o_)
    { assert(!i && !o);
      i = &i_;
      o = &o_;
      cg.w();
      return *this; }

  template<class R2, class F2>
  φ &operator()(φ<R2, F2> &f)
    { let i = new ζ(l, f.b);
      let o = new ζ(l, b);
      (*(c    = &f))  (*o, *i);
      (*(c->c = this))(*i, *o);
      return *this; }


  φ &ω()  { if (i) rω(); if (o) wω();                        return *this; }
  φ &rω() { assert(i); i->rω(); rx();        if (!o) xg.w(); return *this; }
  φ &wω() { assert(o); o->wω(); o = nullptr; if (!i) xg.w(); return *this; }

  φ &rx()
    { if (i && c && c->o) c->wω();
      delete i;
      i = nullptr;
      return *this; }

  operator bool() const { return ri() && wi(); }
  bool     ωi()   const { return !i && !o; }
  bool     ri()   const { return  i && i->ri(); }
  bool     wi()   const { return  o && o->wi(); }
  bool     ra()   const { return  i && i->ra(); }
  bool     wa()   const { return  o && o->wa(); }

  φ &wrca() { while (!ri()) cg.y(λs::φc); return *this; }
  φ &wra()  { wrca(); i->wra();           return *this; }


  template<class X>
  bool operator<<(X const &x) { while (!wi()) cg.y(λs::φc); return o->w(f.w(x, *o)); }

  template<class X>
  bool operator<<=(X const &x) { return wi() && o->w(f.w(x, *o)); }


  R  operator* () const { wra();             return f.r(R(*i + i->a()), *i); }
  φ &operator++()       { wrca(); i->r<R>(); return *this; }


  struct it
  { F &f;
    ζ *z;
    ζp a;
    it   &operator++()                  { a = z->r<R>(); return *this; }
    R     operator* ()            const { return f.r(R(a), *z); }
    bool  operator==(it const &x) const { return a == x.a; } };

  it begin() { wrca(); return it{f, i,       i->r<R>()}; }
  it end()   {         return it{f, nullptr, ζωp}; }
};


#if tau_debug_iostream
template<class R, class W>
O &operator<<(O &s, φ<R, W> const &f)
{
  return s << "φ["
           << (f.i ? *f.i : "∅") << " "
           << (f.o ? *f.o : "∅") << "]";
}
#endif


}

#include "end.hh"


#endif
