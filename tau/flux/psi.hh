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


// ζ state "buffers"
u8c constexpr Ψα = 0x10;
u8c constexpr Ψι = 0x11;
u8c constexpr Ψκ = 0x12;
u8c constexpr Ψω = 0x16;


ζv Ψv(u8c &b)
{
  return std::shared_ptr<i9>(new i9{&b, 0});
}


struct Ψ
{
  struct φ
  { ψi    a,   b;
    ζ<ζv> ab,  ba;
    ζs    sab, sba;
    ΣΘΔ   ra,  rb,  wa,  wb; };

  Λ           &l;
  uN           ζc;
  M<φi, φ>     cs;
  M<ψi, φi>    qs;
  M<ψi, Q<λi>> lr;  // NOTE: queue for fairness
  M<ψi, Q<λi>> lw;
  φi           ci{0};
  ψi           qi{0};

  Ψ(Λ &l_, uN ζc_ = 64) : l(l_), ζc(ζc_) {}

  ψi     ψc ()     { qs[ιi(qi, qs)] = 0;        return qi; }
  bool   ψe (ψi i) { return qs.contains(i); }
  bool   φe (ψi i) { return cs.contains(qs.at(i)); }
  ψi     φo (ψi i) { auto &c = cs.at(qs.at(i)); return c.a == i ? c.b : c.a; }
  ζs    &ψrs(ψi i) { auto &c = cs.at(qs.at(i)); return c.a == i ? c.sba : c.sab; }
  ζs    &ψws(ψi i) { auto &c = cs.at(qs.at(i)); return c.a == i ? c.sab : c.sba; }
  ζ<ζv> &ψrζ(ψi i) { auto &c = cs.at(qs.at(i)); return c.a == i ? c.ba : c.ab; }
  ζ<ζv> &ψwζ(ψi i) { auto &c = cs.at(qs.at(i)); return c.a == i ? c.ab : c.ba; }
  ΣΘΔ   &ψrΘ(ψi i) { auto &c = cs.at(qs.at(i)); return c.a == i ? c.ra : c.rb; }
  ΣΘΔ   &ψwΘ(ψi i) { auto &c = cs.at(qs.at(i)); return c.a == i ? c.wa : c.wb; }

  bool   ψri(ψi i) { return ψrζ(i).ri(); }
  bool   ψwi(ψi i) { return ψwζ(i).wi(); }

  Ψ &λw(Q<λi> &q)
    { while (!q.empty() && !l.e(q.front())) q.pop();
      if (!q.empty()) l.r(q.front(), NAN, λR), q.pop();
      return *this; }

  Ψ &λws(Q<λi> &q)
    { while (!q.empty())
      { if (l.e(q.front())) l.r(q.front(), NAN, λR);
        q.pop(); }
      return *this; }

  bool ψw(ψi i, ζv x)
    { auto &z = ψwζ(i);
      if (!z.xi()) λw(lr[i]);
      return z.w(x); }

  Ψ &ψx(ψi i)
    { auto &z = ψwζ(i); if (z.xi()) return *this;
      let o = φo(i);
      ψws(i) = z.n() ? ζκ : ζω;
      λws(lr[o]); lr.erase(o);
      λws(lw[i]); lw.erase(i);
      z.x();

      return *this; }

  // NOTE: ψrw and ψww are promises to do the actual IO action if possible.
  // This is how we can awaken just one λ per IO.

  // TODO: ζs update is read/write synchronous, so we have the following
  // ownership of updates:
  //
  // reader: α → ι (since the reader is guaranteed a α regardless of timing)
  // writer: ι → κ
  // reader: κ → ω and corresponding φx

  bool ψrw(ψi i)
    { bool  e = ψe(i) && φe(i); if (!e) return false;
      auto &z = ψrζ(i);
      auto &h = ψrΘ(i);
      h.start();

      // TODO: the next loop needs to claim both φ and ψ; that is, it should
      // be impossible for the ψi or φi bindings to change while the loop
      // is running. (It's a security problem if we lose our ζ.)
      //
      // To do this, φs need 128-bit nonce IDs.

      while ((e = ψe(i) && φe(i)) && !z.ri() && !z.xi())
      { if (!l.z()) l.r(l.i(), NAN, λI), lr[i].emplace(l.i());
        λy(); }
      h.stop();
      return e && z.ri(); }

  ζv ψr(ψi i)
    { auto &s = ψrs(i);
      switch (s)
      {
      case ζα: if (φe(i)) s = ζι; return Ψv(Ψα);
      case ζι:
      { auto &z = ψrζ(i);
          if (z.xi()) s = z.ri() ? ζκ : ζω;



      }


      case ζκ:
      { auto &z = ψrζ(i);

      }





      }


      if (z.xi() && !z.n())
      {

      }
      else
        while (!w.empty() && !l.e(w.front())) l.r(w.front(), NAN, λR), w.pop();
      return r; }

  bool ψww(ψi i)
    { auto &z = ψwζ(i);
      auto &h = ψwΘ(i);
      h.start();
      while (!z.wi() && !z.xi())
      { if (!l.z()) l.r(l.i(), NAN, λO);
        lw[i].emplace(l.i());
        λy(); }
      h.stop();
      return z.wi(); }


  φi φc(ψi a, ψi b, uN c = 0)
    { assert(qs.at(a) == 0 && qs.at(b) == 0);
      if (!c) c = ζc;
      cs[qs[a] = qs[b] = ιi(ci, cs)] = {a, b, ζ<ζv>(c), ζ<ζv>(c), ζα, ζα};
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
