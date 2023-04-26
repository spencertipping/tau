#include "auto.hh"
#include "gamma.hh"
#include "begin.hh"

namespace σ
{

using namespace τ;


φ<Γ> φΓa()
{
  static φ<Γ> r = φL<Γ>([]()
    { return φd<Γ>("Γ",
                   "[",  φ1("Γ", φΓ(), Γφwrap(φl("]"))),
                   "=",  φauto(Γpush),
                   "_",  φauto(Γdrop),
                   "??", φauto(Γdebug)); });
  return r;
}


φ<Γ> φΓ()
{
  static φ<Γ> r = φL<Γ>([]()
    { return Γφ(φΓa(), φΨ0(), φΨ1(), φΨ2(), φF<Ψ4>()); });
  return r;
}


Γ Γdrop() { return new Γf_{"_", [](Ξc &x) { return x.drop(); }}; }
Γ Γpush() { return new Γf_{"=", [](Ξc &x) { return x.push({{}, {{}}}); }}; }

Γ Γswap()
{
  return new Γf_{"$$", [](Ξc &x)
    { let [a, y] = x.pop();
      let [b, z] = y.pop();
      return z.push(a).push(b); }};
}


Γ Γvs(St n)
{
  return new Γf_{":" + n, [n](Ξc &x)
    { let [a, y] = x.pop(); return y.vs(n, a); }};
}

Γ Γvg(St n)
{
  return new Γf_{"$" + n, [n](Ξc &x)
    { return x.push(x.vg(n)); }};
}


Γ Γdebug(St tag)
{
  return new Γf_{"debug[" + tag + "]", [tag](Ξc &x)
    { std::cout << "debug " << tag << ": " << x << std::endl;
      return x; }};
}


}

#include "end.hh"
