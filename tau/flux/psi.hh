#ifndef tau_flux_ψ_h
#define tau_flux_ψ_h


#include "../types.hh"
#include "../utf9.hh"
#include "../util/nonce.hh"

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


#if tau_debug_iostream
struct Ψ;
struct φ;

O &operator<<(O &s, φ const &c);
O &operator<<(O &s, Ψ const &q);
#endif


struct φ
{
  nonce n;
  ψi a,  b;
  ζv ab, ba;
};


struct Ψ
{
  Λ           &l;
  uN           ζc;
  M<φi, φ>     cs;
  M<ψi, φi>    qs;
  M<ψi, Q<λi>> lr;  // NOTE: queue for fairness
  M<ψi, Q<λi>> lw;
  M<ψi, Q<λi>> lφc;
  M<ψi, Q<λi>> lφx;
  φi           ci{0};
  ψi           qi{0};
  nonce        ni{0};

  Ψ(Λ &l_, uN ζc_ = ζc0) : l(l_), ζc(ζc_) {}

  ψi   ψc ()           { qs[ιi(qi, qs)] = 0;        return qi; }
  bool ψe (ψi i) const {                            return qs.contains(i); }
  bool φe (ψi i) const {                            return cs.contains(qs.at(i)); }
  ψi   φo (ψi i) const { auto &c = cs.at(qs.at(i)); return c.a == i ? c.b : c.a; }
  ζv  &ψrζ(ψi i)       { auto &c = cs.at(qs.at(i)); return c.a == i ? c.ba : c.ab; }
  ζv  &ψwζ(ψi i)       { auto &c = cs.at(qs.at(i)); return c.a == i ? c.ab : c.ba; }

  ψi ψm(ψi i)
  // NOTE: original ψ is automatically re-bound, so there is no moment
  // for which !ψe
    { assert(φe(i));
      let t = ψc();
      λws(lφx, i); lφx.erase(i);
      qs[t] = qs.at(i); qs[i] = 0;
      lr[t].swap(lr[i]);
      lw[t].swap(lw[i]);
      lφc[t].swap(lφc[i]); // TODO: why would this have data?
      auto &c = cs[qs.at(t)];
      (c.a == i ? c.a : c.b) = t;
      return t; }

  nonce const &ψn(ψi i) const { return cs.at(qs.at(i)).n; }

  template<class T>
  Ψ &λw(M<T, Q<λi>> &qs, T k)
    { if (!qs.contains(k)) return *this;
      auto &q = qs.at(k);
      while (!q.empty() && !l.e(q.front())) q.pop();
      if (!q.empty()) l.r(q.front(), NAN, λR), q.pop();
      return *this; }

  template<class T>
  Ψ &λws(M<T, Q<λi>> &qs, T k)
    { auto &q = qs[k];
      while (!q.empty())
      { if (l.e(q.front())) l.r(q.front(), NAN, λR);
        q.pop(); }
      qs.erase(k);
      return *this; }

  // NOTE: ψrw and ψww are promises to do the actual IO action if possible.
  // This is how we can awaken just one λ per IO.
  bool ψri(ψi i) { return φe(i) && ψrζ(i).ri(); }
  bool ψrw(ψi i)
    { assert(φe(i));
      auto &z = ψrζ(i);
      let   n = ψn(i);  // for security

      // invariant: lr[i] is popped at least once per wakeup of this λ
      while (!z.ri() && !z.xi())
      { if (!l.z()) lr[i].emplace(l.i());
        l.y(λI);
        if (!(ψe(i) && φe(i) && n == ψn(i))) return false; }
      return z.ri(); }

  u9v ψr(ψi i)
    { assert(φe(i));
      let o = φo(i); if (φe(i)) λw(lw, o);
      auto &z = ψrζ(i);
      return z.ri() ? z.r()
           : z.xi() ? u9v{ω}
           :          u9v{κ}; }

  bool ψwi(ψi i) { return φe(i) && ψwζ(i).wi(); }
  bool ψww(ψi i)
    { assert(φe(i));
      auto &z = ψwζ(i);
      let   n = ψn(i);
      while (!z.wi() && !z.xi())
      { if (!l.z()) lw[i].emplace(l.i());
        l.y(λO);
        if (!(ψe(i) && φe(i) && n == ψn(i))) return false; }
      return z.wi(); }

  bool ψw(ψi i, u9v const &x)
    { let o = φo(i);
      if (φe(i)) λw(lr, o);
      return ψwζ(i).w(x); }

  bool ψφw(ψi i)
    { assert(ψe(i));
      while (ψe(i) && !φe(i)) { lφc[i].emplace(l.i()); l.y(λφc); }
      return ψe(i); }

  bool φxw(ψi i)
    { assert(ψe(i));
      while (ψe(i) && φe(i)) { lφx[i].emplace(l.i()); l.y(λφx); }
      return ψe(i); }

  // NOTE: ψx is a promise that no further IO will be done using this
  // port; that is, the port is released and can be invalidated at any
  // time (when both sides of φ are closed, φ is freed)
  Ψ &ψx(ψi i)
    { assert(ψe(i));
      if (φe(i))
      { auto &z = ψwζ(i);
        if (z.xi()) return *this;

        let o = φo(i);
        λws(lr, o);    // awaken blocked readers on other side (to fail)
        λws(lw, i);    // awaken blocked writers on this side (to fail)
        λws(lφx, i);   // this ψ no longer has φ

        z.w(ω, true);  // send final ω
        z.x();         // close

        assert(!lφc.contains(i) || lφc.at(i).empty());
        lφc.erase(i);

        if (ψe(o) && φe(o) && ψwζ(o).xi())
        { λws(lr, i);
          λws(lw, o);
          φx(qs.at(i));
          assert(!lφc.contains(o) || lφc.at(o).empty());
          lφc.erase(o); } }
      else
      { λws(lw, i);
        λws(lr, i);
        λws(lφc, i); }

      qs.erase(i);
      return *this; }

  φi φc(ψi a, ψi b, uN c = 0)
    { if (!φxw(a) || !φxw(b)) return 0;
      if (!c) c = ζc;
      auto &f = cs[qs[a] = qs[b] = ιi(ci, cs)] = {++ni, a, b, ζv(c), ζv(c)};
      f.ab.w(α, true);
      f.ba.w(α, true);
      λw(lφc, a);
      λw(lφc, b);
      return ci; }

  Ψ &φx(φi i)
    { if (i)
      { let &c = cs.at(i);
        λw(lφx, c.a);
        λw(lφx, c.b);
        cs.erase(i); }
      return *this; }
};


#if tau_debug_iostream
O &operator<<(O &s, φ const &c)
{
  return s << "φ["
           << c.a << "-" << c.b << " " << c.n << " "
           << c.ab << " " << c.ba << "]";
}


O &operator<<(O &s, Ψ const &q)
{
  s << "Ψ ζc=" << q.ζc << std::endl;
  for (let &[_, c] : q.cs) s << "  " << c << std::endl;
  return s;
}
#elif tau_debug_nop
O &operator<<(O &s, φ const &c) { return s; }
O &operator<<(O &s, Ψ const &q) { return s; }
#endif


}

#include "../module/end.hh"


#endif
