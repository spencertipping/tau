#include "Gamma-phi.hh"
#include "pi-phi.hh"
#include "begin.hh"

namespace τ
{


φ<Γ> ΓφΨdir(Stc &n, Ψd d,
            φ<Ψ0> p0, φ<Ψ1> p1, φ<Ψ2> p2, φ<Ψ4> p4)
{
  let p = φa(n,
             φM(p0, [d](φc_ const &c, φr_<Ψ0> &&s) { return s.cast(ΓΨ(mo(s).r(), d, c.sub(s))); }),
             φM(p1, [d](φc_ const &c, φr_<Ψ1> &&s) { return s.cast(ΓΨ(mo(s).r(), d, c.sub(s))); }),
             φM(p2, [d](φc_ const &c, φr_<Ψ2> &&s) { return s.cast(ΓΨ(mo(s).r(), d, c.sub(s))); }),
             φM(p4, [d](φc_ const &c, φr_<Ψ4> &&s) { return s.cast(ΓΨ(mo(s).r(), d, c.sub(s))); }));

  // Accept one, or assign the same direction to many
  return p | φm(φgroup(φn(p, 1)), Γs);
}


}

#include "end.hh"
