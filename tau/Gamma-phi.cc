#include "Gamma-phi.hh"
#include "pi-phi.hh"
#include "begin.hh"

namespace τ
{


φ<St> Γφws() { return πφws(); }
φ<St> Γφlc() { return πφlc(); }
φ<St> Γφig() { return πφig(); }

φ<St> Γφlb() { slet r = Γφwrap(φl("[")); return r; }
φ<St> Γφrb() { slet r = Γφwrap(φl("]")); return r; }


static φ<Γ> ΓφΨ(St    n,
                φ<Ψ0> a,
                φ<Ψ1> b,
                φ<Ψ2> c,
                φ<Ψ4> d,
                Ψd    e)
{
  return φa(n,
            φM(a, [e](φc_ const &c, φr_<Ψ0> &&s) -> φr_<Γ> { return s.cast(ΓΨ(mo(s).r(), e, c.sub(s))); }),
            φM(b, [e](φc_ const &c, φr_<Ψ1> &&s) -> φr_<Γ> { return s.cast(ΓΨ(mo(s).r(), e, c.sub(s))); }),
            φM(c, [e](φc_ const &c, φr_<Ψ2> &&s) -> φr_<Γ> { return s.cast(ΓΨ(mo(s).r(), e, c.sub(s))); }),
            φM(d, [e](φc_ const &c, φr_<Ψ4> &&s) -> φr_<Γ> { return s.cast(ΓΨ(mo(s).r(), e, c.sub(s))); }));
}


P<φ<Γ>, φ<Γ>> Γφ(φ<Γ> g, φ<Ψ0> a, φ<Ψ1> b, φ<Ψ2> c, φ<Ψ4> d)
{
  let g1 = φa0<Γ>("Γ₁");  auto &g1a = g1.as<φa_<Γ>>();
  let gs = φm(φn(Γφwrap(g1)), [](V<Γ> gs) -> Γ
    { if (gs.empty()) return new Γ_;  // identity
      Γ r = gs.back();
      for (iN i = gs.size() - 2; i >= 0; --i) r = gs[i] | r;
      return r; });
  let gw = φW(gs);

  let wa = Γφwrap(a);
  let wb = Γφwrap(b);
  let wc = Γφwrap(c);
  let wd = Γφwrap(d);

  // TODO: \ and | should support [] groups

  g1a << Γφgroup(gw)
      << Γφwrap(g)
      << φ2("Γ|",  Γφwrap(φl("|")),  ΓφΨ("Γ|",  wa, wb, wc, wd, Ψd::r))
      << φ2("Γ\\", Γφwrap(φl("\\")), ΓφΨ("Γ\\", wa, wb, wc, wd, Ψd::b))
      << ΓφΨ("ΓΨ", wa, wb, wc, wd, Ψd::f);

  return {g1, gs};
}


}

#include "end.hh"
