#ifndef tau_flux_psi_h
#define tau_flux_psi_h


#include "../types.hh"
#include "../utf9.hh"
#include "../util/nonce.hh"
#include "../util/shd.hh"

#include "types.hh"
#include "u9z.hh"
#include "zeta.hh"
#include "lambda.hh"
#include "Lambda.hh"


#include "../module/begin.hh"

namespace tau::flux
{


using tau::util::nonce;


typedef ζ<u9v, u9vs> ζv;


struct Ψ
{
  struct φ
  { nonce n;
    ψi  a,  b;
    ζv  ab, ba;
    ΣΘΔ ra, rb, wa, wb; };

  Λ           &l;
  uN           ζc;
  M<φi, φ>     cs;
  M<ψi, φi>    qs;
  M<ψi, Q<λi>> lr;  // NOTE: queue for fairness
  M<ψi, Q<λi>> lw;
  φi           ci{0};
  ψi           qi{0};
  nonce        ni{0};

  Ψ(Λ &l_, uN ζc_ = ζc₀) : l(l_), ζc(ζc_) {}

  ψi   ψc ()           { qs[ιi(qi, qs)] = 0;        return qi; }
  bool ψe (ψi i) const {                            return qs.contains(i); }
  bool φe (ψi i) const {                            return cs.contains(qs.at(i)); }
  ψi   φo (ψi i) const { auto &c = cs.at(qs.at(i)); return c.a == i ? c.b : c.a; }
  ζv  &ψrζ(ψi i)       { auto &c = cs.at(qs.at(i)); return c.a == i ? c.ba : c.ab; }
  ζv  &ψwζ(ψi i)       { auto &c = cs.at(qs.at(i)); return c.a == i ? c.ab : c.ba; }
  ΣΘΔ &ψrΘ(ψi i)       { auto &c = cs.at(qs.at(i)); return c.a == i ? c.ra : c.rb; }
  ΣΘΔ &ψwΘ(ψi i)       { auto &c = cs.at(qs.at(i)); return c.a == i ? c.wa : c.wb; }

  nonce const &ψn(ψi i) const { return cs.at(qs.at(i)).n; }

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
  bool ψri(ψi i) { return ψrζ(i).ri(); }
  bool ψrw(ψi i)
    { auto &h = ψrΘ(i);
      auto &z = ψrζ(i);
      let   n = ψn(i);  // for security

      // invariant: lr[i] is popped at least once per wakeup of this λ
      h.start();
      while (!z.ri() && !z.xi())
      { if (!l.z()) lr[i].emplace(l.i());
        l.y(λI);
        if (!(ψe(i) && φe(i) && n == ψn(i))) return false; }
      h.stop();
      return z.ri(); }

  u9v ψr(ψi i)
    { assert(φe(i));
      let o = φo(i); if (φe(i) && lw.contains(o)) λw(lw[o]);
      auto &z = ψrζ(i);
      return z.ri() ? z.r()
           : z.xi() ? u9v{ω}
           :          u9v{κ}; }

  bool ψwi(ψi i) { return ψwζ(i).wi(); }
  bool ψww(ψi i)
    { auto &z = ψwζ(i);
      auto &h = ψwΘ(i);
      let   n = ψn(i);
      h.start();
      while (!z.wi() && !z.xi())
      { if (!l.z()) lw[i].emplace(l.i());
        l.y(λO);
        if (!(ψe(i) && φe(i) && n == ψn(i))) return false; }
      h.stop();
      return z.wi(); }

  bool ψw(ψi i, u9v const &x)
    { let o = φo(i);
      if (φe(i) && lr.contains(o)) λw(lr[o]);
      return ψwζ(i).w(x); }

  // NOTE: ψx is a promise that no further IO will be done using this
  // port; that is, the port is released and can be invalidated at any
  // time (when both sides of φ are closed, φ is freed)
  Ψ &ψx(ψi i)
    { assert(ψe(i));
      auto &z = ψwζ(i);
      if (z.xi()) return *this;

      let o = φo(i);
      λws(lr[o]); lr.erase(o);
      λws(lw[i]); lw.erase(i);
      z.w(ω, true);
      z.x();

      if (ψwζ(o).xi())
      { λws(lr[i]); lr.erase(i);
        λws(lw[o]); lw.erase(o);
        φx(qs.at(i));
        qs.erase(i); qs.erase(o); }

      return *this; }


  φi φc(ψi a, ψi b, uN c = 0)
    { assert(qs.at(a) == 0 && qs.at(b) == 0);
      if (!c) c = ζc;
      auto &f = cs[qs[a] = qs[b] = ιi(ci, cs)] = {++ni, a, b, ζv(c), ζv(c)};
      f.ab.w(α, true);
      f.ba.w(α, true);
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
  return s << "φ["
           << c.a << "-" << c.b << " " << c.n << " "
           << c.ab << " " << c.ba << "]";
}


O &operator<<(O &s, Ψ const &q)
{
  s << "Ψ ζc=" << q.ζc << std::endl;
  for (let &[k, c] : q.qs)
    s << "  " << k << "\t" << q.cs.at(c) << std::endl;
  return s;
}
#endif


}

#include "../module/end.hh"


#endif
