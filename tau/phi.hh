#ifndef τφ_h
#define τφ_h


#include "gate.hh"
#include "types.hh"
#include "zeta.hh"
#include "Sigma.hh"
#include "shd.hh"

#include "begin.hh"

namespace τ
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
  Σι is, os;
  φπ() { i.start(); o.start(); }
  template<class R> R          r(R x, ζ<R>&) { i.stop(); i.start(); is << x.size(); return x; }
  template<class W, class Z> W w(W x, ζ<Z>&) { o.stop(); o.start(); os << x.size(); return x; }
};


template<class R, class W = R, class F = φι>
struct φ_
{
  sletc fnr = 0x1;   // no read-side
  sletc fnw = 0x2;   // no write-side
  sletc fli = 0x4;   // lock when reading
  sletc flo = 0x8;   // lock when writing
  sletc fie = 0x10;  // input is ended (closed permanently)
  sletc foe = 0x20;  // output is ended

  Λ           &l;
  φ_<W, R, F> *c {nullptr};  // NOTE: template args may vary; ptr type is a lie
  u8c          b;
  u8           fs;
  ζ<R>        *i {nullptr};
  ζ<W>        *o {nullptr};
  λg           cg;
  λg           xg;
  F            f;  // NOTE: must be placed last due to F erasure


  φ_(φ_ &) = delete;
  φ_(φ_&&) = delete;
  φ_(Λ &l_, uf8 b_ = 0, u8 fs_ = 0)
    : l{l_}, b{b_ ? b_ : ζb0}, fs{fs_}, cg{l}, xg{l} {}

  ~φ_()
    { ω();
      if (c) c->c = nullptr;
      else
      { if (i) delete i;
        if (o) delete o; } }


  φ_ &operator()(ζ<R> &i_, ζ<W> &o_)
    { assert(!i && !o);
      i = &i_;
      o = &o_;
      cg.w();
      return *this; }

  template<class F2>
  φ_ &operator()(φ_<W, R, F2> &f)
    { let p = Rc<φ_<W, R, F>*>(&f);
      let i = !(fs & fnr) && !(p->fs & fnw) && p->b
            ? new ζ<R>(l, p->b, fs & fli || p->fs & flo ? ζ<R>::fl : 0)
            : nullptr;
      let o = !(fs & fnw) && !(p->fs & fnr) && b
            ? new ζ<W>(l, b,    fs & flo || p->fs & fli ? ζ<W>::fl : 0)
            : nullptr;
      (*(c    = p))   (*o, *i);
      (*(c->c = this))(*i, *o);
      return *this; }


  φ_ &ω()  { rω(); wω();                               fs |= fie | foe; return *this; }
  φ_ &rω() { if (i) { i->rω(); rx(); if (!o) xg.w(); } fs |= fie;       return *this; }
  φ_ &wω() { if (o) { o->wω();       if (!i) xg.w(); } fs |= foe;       return *this; }

  φ_ &rx()
    { if (i) { if (c) c->wω(); i->wω().rω(); }
      return *this; }

  operator bool() const { return ri() && wi(); }
  bool     ωi()   const { return   !i && !o; }
  bool     ri()   const { return !(fs & fie) && i && i->ri(); }
  bool     wi()   const { return !(fs & foe) && o && o->wi(); }
  bool     ra()   const { return !(fs & fie) && i && i->ra(); }
  bool     wa()   const { return !(fs & foe) && o && o->wa(); }

  φ_ &wrc() { while (!ri()) cg.y(λs::φc); return *this; }
  φ_ &wra() { wrc(); i->wra();            return *this; }


  template<class X>  // write, wait for ζ if necessary
  bool operator<<(X const &x)
    { while (!(fs & foe) && !wi()) cg.y(λs::φc);
      return fs & foe ? false : *o << f.w(x, *o); }


  template<class X>  // write but don't wait for ζ
  bool operator<<=(X const &x) { return wi() && *this << x; }


  φ_ &operator++() { ++*i;  return *this; }
  R   operator* () { wra(); return f.r(R(**i), *i); }


  struct it
  { φ_   &f;
    bool  eof;
    R     operator* ()            const { return *f; }
    bool  operator==(it const &x) const { return eof == x.eof; }
    it   &operator++() { ++f; eof = !f.i->wra(); return *this; } };

  it begin() { wra(); return it{*this, !ra()}; }
  it end()   {        return it{*this, true}; }
};


#if τdebug_iostream
template<class R, class W, class F>
O &operator<<(O &s, φ_<R, W, F> const &f)
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
