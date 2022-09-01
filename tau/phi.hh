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
  template<class R>          ic R r(R x, ζ<R>&) { return x; }
  template<class W, class Z> ic W w(W x, ζ<Z>&) { return x; }
};


struct φc  // capacity check filter
{
  u64 om  = 0;
  u64 ohc = 0;

  template<class R>          ic R r(R x, ζ<R>&) { return x; }
  template<class W, class Z> ic W w(W x, ζ<Z>& z)
    { if (x.size() >= z.b.c >> 1) ohc++;
      om++;
      return x; }
};


template<class Θ>
struct φπ  // profiling+timing filter
{
  Θ  i,  o;
  πι is, os;
  φπ() { i.start(); o.start(); }
  template<class R> R          r(R x, ζ<R>&) { i.stop(); i.start(); is << x.size(); return x; }
  template<class W, class Z> W w(W x, ζ<Z>&) { o.stop(); o.start(); os << x.size(); return x; }
};


template<class R, class W = R, class F = φι>
struct φ
{
  Λ       &l;
  φ<W, R, F> *c {nullptr};  // NOTE: template args may vary; ptr type is a lie
  uf8c     b;
  ζ<R>    *i {nullptr};
  ζ<W>    *o {nullptr};
  λg       cg;
  λg       xg;
  F        f;


  φ(φ &) = delete;
  φ(Λ &l_, uf8 b = ζb0) : l{l_}, b{b}, cg{l}, xg{l} {}
  ~φ()
    { rx();
      if (c) c->c = nullptr; }


  φ &operator()(ζ<R> &i_, ζ<W> &o_)
    { assert(!i && !o);
      i = &i_;
      o = &o_;
      cg.w();
      return *this; }

  template<class F2>
  φ &operator()(φ<W, R, F2> &f)
    { let p = Rc<φ<W, R, F>*>(&f);
      let i = new ζ<R>(l, p->b);
      let o = new ζ<W>(l, b);
      (*(c    = p))   (*o, *i);
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
  bool     ωi()   const { return   !i && !o; }
  bool     ri()   const { return    i && i->ri(); }
  bool     wi()   const { return    o && o->wi(); }
  bool     ra()   const { return    i && i->ra(); }
  bool     wa()   const { return    o && o->wa(); }

  φ &wrc() { while (!ri()) cg.y(λs::φc); return *this; }
  φ &wra() { wrc(); i->wra();            return *this; }


  template<class X>  // blocking write
  bool operator<<(X const &x)
    { while (!wi()) cg.y(λs::φc);
      return *o << f.w(x, *o); }

  template<class X>  // non-blocking write
  bool operator<<=(X const &x)
    { if (!wi() || !wa()) return false;
      auto y = f.w(x, *o);
      return y.size() <= o->b.wa() && *o << y; }


  φ &operator++() { ++*i;  return *this; }
  R  operator* () { wra(); return f.r(R(**i), *i); }


  struct it
  { φ    &f;
    bool  eof;
    R     operator* ()            const { return *f; }
    bool  operator==(it const &x) const { return eof == x.eof; }
    it   &operator++() { ++f; f.i->wra(); eof = !f.ri(); return *this; } };

  it begin() { wra(); return it{*this, !ra()}; }
  it end()   {        return it{*this, true}; }
};


#if tau_debug_iostream
template<class R, class W, class F>
O &operator<<(O &s, φ<R, W, F> const &f)
{
  s << "φ[";
  if (f.i) s << *f.i << " "; else s << "∅";
  if (f.o) s << *f.o;        else s << "∅";
  return s << "]";
}
#endif


}

#include "end.hh"


#endif
