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


φ<Γ> Γφ(φ<Γ> g, φ<Ψ0> a, φ<Ψ1> b, φ<Ψ2> c, φ<Ψ4> d)
{
  let g1 = φa0<Γ>("Γ₁");  auto &g1a = g1.as<φa_<Γ>>();
  let gs = φm(φn(g1), [](V<Γ> gs) -> Γ
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
      << φ2("Γ|", Γφwrap(φl("|")),
            φa("Γ|",
               φm(wa, [](Ψ0 x) -> Γ { return new ΓΨ0(mo(x), Ψd::r, "ΓΨ₀"); }),
               φm(wb, [](Ψ1 x) -> Γ { return new ΓΨ1(mo(x), Ψd::r, "ΓΨ₁"); }),
               φm(wc, [](Ψ2 x) -> Γ { return new ΓΨ2(mo(x), Ψd::r, "ΓΨ₂"); }),
               φm(wd, [](Ψ4 x) -> Γ { return new ΓΨ4(mo(x), Ψd::r, "ΓΨ₄"); })))
      << φ2("Γ\\", Γφwrap(φl("\\")),
            φa("Γ\\",
               φm(wa, [](Ψ0 x) -> Γ { return new ΓΨ0(mo(x), Ψd::b, "ΓΨ₀"); }),
               φm(wb, [](Ψ1 x) -> Γ { return new ΓΨ1(mo(x), Ψd::b, "ΓΨ₁"); }),
               φm(wc, [](Ψ2 x) -> Γ { return new ΓΨ2(mo(x), Ψd::b, "ΓΨ₂"); }),
               φm(wd, [](Ψ4 x) -> Γ { return new ΓΨ4(mo(x), Ψd::b, "ΓΨ₄"); })))
      << φm(wa, [](Ψ0 x) -> Γ { return new ΓΨ0(mo(x), Ψd::f, "ΓΨ₀"); })
      << φm(wb, [](Ψ1 x) -> Γ { return new ΓΨ1(mo(x), Ψd::f, "ΓΨ₁"); })
      << φm(wc, [](Ψ2 x) -> Γ { return new ΓΨ2(mo(x), Ψd::f, "ΓΨ₂"); })
      << φm(wd, [](Ψ4 x) -> Γ { return new ΓΨ4(mo(x), Ψd::f, "ΓΨ₄"); });

  return gs;
}


}

#include "end.hh"
