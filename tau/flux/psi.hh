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

  Ψ(Λ &l_, uN ζc = 64) : l(l_), c_(ζc) {}

  ψi     ψc()      { qs[ιi(qi, qs)] = 0;        return qi; }
  ζ<ζv> &ψrζ(ψi i) { auto &c = cs.at(qs.at(i)); return c.a == i ? c.ba : c.ab; }
  ζ<ζv> &ψwζ(ψi i) { auto &c = cs.at(qs.at(i)); return c.a == i ? c.ab : c.ba; }
  ΣΘΔ   &ψrΘ(ψi i) { auto &c = cs.at(qs.at(i)); return c.a == i ? c.ra : c.rb; }
  ΣΘΔ   &ψwΘ(ψi i) { auto &c = cs.at(qs.at(i)); return c.a == i ? c.wa : c.wb; }

  bool   ψri(ψi i) { return ψrζ(i).ri(); }
  bool   ψwi(ψi i) { return ψwζ(i).wi(); }

  ζv ψr(ψi i)
    { for (let j : lw[i]) { l.r(j, NAN, λR); break; }
      auto &z = ψrζ(i);
      let   r = z.r();
      if (z.xi() && !z.n())
      {
        // TODO: do _something_ here, but what exactly?
      }
      return r; }

  bool ψw(ψi i, ζv x)
    { for (let j : lr[i]) { l.r(j, NAN, λR); break; }
      return ψwζ(i).w(x); }

  // TODO: define what it means to close a port
  // Closing the writer and closing the port are two different actions.
  //
  // I think we need to model half-open states.

  // FIXME: delete φ iff both directions are closed and all contents
  // have been consumed (maybe assert this)
  //
  // TODO: wake readers and writers
  //
  // i.e. this function should correspond to a γ user-close, not a kernel
  // close.
  Ψ &ψx(ψi i)
    {
      lr.erase(i);
      lw.erase(i);
      φx(qs.at(i));
      qs.erase(i);
      return *this; }

  // NOTE: ψrw and ψww are promises to do the actual IO action if possible.
  // This is how we can awaken just one λ per IO.
  bool ψrw(ψi i)
    { auto &z = ψrζ(i);
      auto &h = ψrΘ(i);
      h.start();
      while (!z.ri() && !z.xi())
      { if (!l.z()) l.r(l.ri, NAN, λI);
        lr[i].emplace(l.ri);
        λy(); }
      h.stop();
      return z.ri(); }

  bool ψww(ψi i)
    { auto &z = ψwζ(i);
      auto &h = ψwΘ(i);
      h.start();
      while (!z.wi() && !z.xi())
      { if (!l.z()) l.r(l.ri, NAN, λO);
        lw[i].emplace(l.ri);
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
