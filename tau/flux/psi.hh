#ifndef tau_flux_psi_h
#define tau_flux_psi_h


#include "../types.hh"
#include "../util/shd.hh"

#include "types.hh"
#include "zeta.hh"
#include "lambda.hh"
#include "Lambda.hh"


#include "../module/begin.hh"

namespace tau::flux
{


struct Ψ
{
  struct φ
  { ψi    a,  b;
    ζ<ζv> ab, ba;
    ΣΘΔ   ra, rb, wa, wb; };

  Λ           &l;
  uN           c_;
  M<φi, φ>     cs;
  M<ψi, φi>    qs;
  M<ψi, S<λi>> lr;
  M<ψi, S<λi>> lw;
  φi           ci{0};
  ψi           qi{0};

  Ψ(Λ &l_, uN c__ = 64) : l(l_), c_(c__) {}

  ψi     ψc()      { qs[ιi(qi, qs)] = 0;        return qi; }
  ζ<ζv> &ψrζ(ψi i) { auto &c = cs.at(qs.at(i)); return c.a == i ? c.ba : c.ab; }
  ζ<ζv> &ψwζ(ψi i) { auto &c = cs.at(qs.at(i)); return c.a == i ? c.ab : c.ba; }
  ΣΘΔ   &ψrΘ(ψi i) { auto &c = cs.at(qs.at(i)); return c.a == i ? c.ra : c.rb; }
  ΣΘΔ   &ψwΘ(ψi i) { auto &c = cs.at(qs.at(i)); return c.a == i ? c.wa : c.wb; }

  bool   ψri(ψi i)       { return ψrζ(i).ri(); }
  bool   ψwi(ψi i)       { return ψwζ(i).wi(); }
  ζv     ψr (ψi i)       { return ψrζ(i).r();  } // TODO: auto-close
  bool   ψw (ψi i, ζv x) { return ψwζ(i).w(x); }

  // FIXME: delete φ iff both directions are closed and all contents
  // have been consumed
  Ψ &ψx (ψi i)
    { lr.erase(i);
      lw.erase(i);
      let c = qs.at(i);
      φx(c);
      qs.erase(i);
      return *this; }

  bool ψrw(ψi i)
    { auto &z = ψrζ(i);
      auto &h = ψrΘ(i);
      h.start();
      while (!z.ri() && !z.ci())
      { if (!l.z()) l.r(l.ri, NAN, λI);
        lr[i].emplace(l.ri);
        for (let j : lw[i]) l.r(j, NAN, λR);
        lw.erase(i);
        λy(); }
      h.stop();
      return z.ri(); }

  bool ψww(ψi i)
    { auto &z = ψwζ(i);
      auto &h = ψwΘ(i);
      h.start();
      while (!z.wi() && !z.ci())
      { if (!l.z()) l.r(l.ri, NAN, λO);
        lw[i].emplace(l.ri);
        for (let j : lr[i]) l.r(j, NAN, λR);
        lr.erase(i);
        λy(); }
      h.stop();
      return z.wi(); }


  φi φc(ψi a, ψi b, uN c = 0)
    { assert(qs.at(a) == 0 && qs.at(b) == 0);
      if (!c) c = c_;
      cs[qs[a] = qs[b] = ιi(ci, cs)] = {a, b, ζ<ζv>(c), ζ<ζv>(c)};
      return ci; }

  Ψ &φx(φi i)
    { if (i)
      { let &c = cs.at(i);
        qs.erase(c.a); qs.erase(c.b);
        cs.erase(i); }
      return *this; }
};


#if tau_debug_iostream
O &operator<<(O &s, Ψ::φ const &c)
{
  return s << "TODO: φ";
}


O &operator<<(O &s, Ψ const &q)
{
  return s << "TODO: Ψ";
}
#endif


}

#include "../module/end.hh"


#endif
