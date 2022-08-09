#ifndef tau_flux_psi_h
#define tau_flux_psi_h


#include "../types.hh"
#include "../utf9.hh"
#include "../util/nonce.hh"
#include "../util/shd.hh"

#include "types.hh"
#include "zeta.hh"
#include "lambda.hh"
#include "Lambda.hh"


#include "../module/begin.hh"

namespace tau::flux
{


using tau::util::nonce;


// ζ state "buffers"
u8c constexpr Ψα = 0x10;
u8c constexpr Ψι = 0x11;
u8c constexpr Ψκ = 0x12;
u8c constexpr Ψω = 0x16;


ζv Ψv(u8c &b)
{
  return std::shared_ptr<i9>(new i9{&b, 1});
}


ζv Ψv(u9 const &x)
{
  o9 b;
  auto *r = new i9((b << x).convert_to_ibuf());
  return std::shared_ptr<i9>(r);
}


struct Ψ
{
  struct φ
  { nonce n;
    ψi    a,   b;
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
  nonce        ni{0};

  Ψ(Λ &l_, uN ζc_ = 64) : l(l_), ζc(ζc_) {}

  ψi     ψc ()     { qs[ιi(qi, qs)] = 0;        return qi; }
  bool   ψe (ψi i) {                            return qs.contains(i); }
  bool   φe (ψi i) {                            return cs.contains(qs.at(i)); }
  ψi     φo (ψi i) { auto &c = cs.at(qs.at(i)); return c.a == i ? c.b : c.a; }
  ζs    &ψrs(ψi i) { auto &c = cs.at(qs.at(i)); return c.a == i ? c.sba : c.sab; }
  ζs    &ψws(ψi i) { auto &c = cs.at(qs.at(i)); return c.a == i ? c.sab : c.sba; }
  ζ<ζv> &ψrζ(ψi i) { auto &c = cs.at(qs.at(i)); return c.a == i ? c.ba : c.ab; }
  ζ<ζv> &ψwζ(ψi i) { auto &c = cs.at(qs.at(i)); return c.a == i ? c.ab : c.ba; }
  ΣΘΔ   &ψrΘ(ψi i) { auto &c = cs.at(qs.at(i)); return c.a == i ? c.ra : c.rb; }
  ΣΘΔ   &ψwΘ(ψi i) { auto &c = cs.at(qs.at(i)); return c.a == i ? c.wa : c.wb; }

  bool   ψri(ψi i) { return ψrζ(i).ri(); }
  bool   ψwi(ψi i) { return ψwζ(i).wi(); }

  nonce const &ψn (ψi i) { return cs.at(qs.at(i)).n; }

  Ψ &λw(Q<λi> &q)
    { while (!q.empty() && !l.e(q.front())) q.pop();
      if (!q.empty()) l.r(q.front(), NAN, λR), q.pop();
      return *this; }

  Ψ &λws(Q<λi> &q)
    { while (!q.empty())
      { if (l.e(q.front())) l.r(q.front(), NAN, λR);
        q.pop(); }
      return *this; }

  // NOTE: ψrw and ψww are promises to do the actual IO action if possible.
  // This is how we can awaken just one λ per IO.

  // ζs update is read/write synchronous, so we have the following
  // ownership of updates:
  //
  // reader: α → ι|κ|ω (since the reader is guaranteed an α regardless of timing)
  // writer: ι → κ|ω (since only the writer can call ζ.x())
  // reader: κ → ω and corresponding φx

  bool ψrw(ψi i)
    { bool  e = ψe(i) && φe(i); if (!e) return false;
      auto &h = ψrΘ(i);
      auto &z = ψrζ(i);
      let   n = ψn(i);  // for security

      // invariant: lr[i] is popped at least once per wakeup of this λ
      h.start();
      while ((e = ψe(i) && φe(i) && n == ψn(i)) && !z.ri() && !z.xi())
      { if (!l.z()) l.r(l.i(), NAN, λI), lr[i].emplace(l.i());
        λy(); }
      h.stop();

      return e && z.ri(); }

  ζs ζrs(ζ<ζv> const &z) const { return z.xi() ? z.n() ? ζκ : ζω : ζι; }
  ζv ψr(ψi i)
    { if (!ψe(i) || !φe(i)) return Ψv(Ψω);
      let o = φo(i);
      if (φe(i) && lw.contains(o)) λw(lw[o]);
      auto &s = ψrs(i);
      switch (s)
      {
      case ζα: if (φe(i)) s = ζrs(ψrζ(i)); return Ψv(Ψα);
      case ζι:
      case ζκ: { auto &z = ψrζ(i); let v = z.r(); s = ζrs(z); return v; }
      case ζω: return Ψv(Ψω);
      default: return nullptr;  // unreachable
      } }

  bool ψww(ψi i)
    { bool  e = ψe(i) && φe(i); if (!e) return false;
      auto &z = ψwζ(i);
      auto &h = ψwΘ(i);
      let   n = ψn(i);
      h.start();
      while ((e = ψe(i) && φe(i) && n == ψn(i)) && !z.wi() && !z.xi())
      { if (!l.z()) l.r(l.i(), NAN, λI), lw[i].emplace(l.i());
        λy(); }
      h.stop();
      return e && z.wi(); }

  bool ψw(ψi i, ζv x)
    { let o = φo(i);
      if (φe(i) && lr.contains(o)) λw(lr[o]);
      return ψwζ(i).w(x); }

  // TODO: change this logic; we should close when both sides have closed,
  // regardless of whether data is in the pipes
  Ψ &ψx(ψi i)  // close the writer from this side of the φ pair
    { if (!ψe(i) || !φe(i)) return *this;
      auto &z = ψwζ(i); if (z.xi()) return *this;
      let   o = φo(i);

      // wake any readers on the other side, and any writers on this side
      λws(lr[o]); lr.erase(o);
      λws(lw[i]); lw.erase(i);
      z.x();
      ψws(i) = z.n() ? ζκ : ζω;

      if (!z.n() && ψe(o) && ψws(o) == ζω) { φx(qs.at(i)); qs.erase(i); qs.erase(o); }
      return *this; }


  φi φc(ψi a, ψi b, uN c = 0)
    { assert(qs.at(a) == 0 && qs.at(b) == 0);
      if (!c) c = ζc;
      cs[qs[a] = qs[b] = ιi(ci, cs)] = {++ni, a, b, ζ<ζv>(c), ζ<ζv>(c), ζα, ζα};
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
