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
            φm(a, [e](Ψ0 x) -> Γ { return ΓΨ(mo(x), e, "ΓΨ₀"); }),
            φm(b, [e](Ψ1 x) -> Γ { return ΓΨ(mo(x), e, "ΓΨ₁"); }),
            φm(c, [e](Ψ2 x) -> Γ { return ΓΨ(mo(x), e, "ΓΨ₂"); }),
            φm(d, [e](Ψ4 x) -> Γ { return ΓΨ(mo(x), e, "ΓΨ₄"); }));
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
