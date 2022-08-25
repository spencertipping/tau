#ifndef tau_φ_h
#define tau_φ_h


#include "gate.hh"
#include "types.hh"
#include "zeta.hh"

#include "begin.hh"

namespace tau
{


struct φany
{
  template<class W> static W cast(W x) { return x; }
};


template<class R, class W = φany>
struct φ
{
  ζ *i {nullptr};
  ζ *o {nullptr};
  λg cg;
  λg xg;


  φ(φ &) = delete;
  φ(Λ &l) : cg{l}, xg{l} {}


  φ &operator()(ζ &i_, ζ &o_)
    { assert(!i && !o);
      i = &i_;
      o = &o_;
      cg.w();
      return *this; }


  φ &ω()  { if (i) rω(); if (o) wω();                        return *this; }
  φ &rω() { assert(i); i->rω(); i = nullptr; if (!o) xg.w(); return *this; }
  φ &wω() { assert(o); o->wω(); o = nullptr; if (!i) xg.w(); return *this; }

  operator bool() const { return ri() && wi(); }
  bool     ωi()   const { return !i && !o; }
  bool     ri()   const { return  i && i->ri(); }
  bool     wi()   const { return  o && o->wi(); }
  bool     ra()   const { return  i && i->ra(); }
  bool     wa()   const { return  o && o->wa(); }

  φ &wrca() { while (!ri()) cg.y(λs::φc); return *this; }
  φ &wra()  { wrca(); i->wra();           return *this; }


  template<class X>
  bool operator<<(X const &x)
    { while (!wi()) cg.y(λs::φc);
      return o->w(W::cast(x)); }


  R operator*() const { wra();             return R(*i + i->a()); }
  φ &operator++()     { wrca(); i->r<R>(); return *this; }


  struct it
  { ζ *z;
    ζp a;
    it   &operator++()                  { a = z->r<R>(); return *this; }
    R     operator* ()            const { return R(a); }
    bool  operator==(it const &x) const { return a == x.a; } };

  it begin()       { wrca(); return it{i,       i->r<R>()}; }
  it end()   const {         return it{nullptr, ζωp}; }
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
