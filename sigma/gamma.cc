#include "auto.hh"
#include "gamma.hh"
#include "begin.hh"

namespace σ
{

using namespace τ;


static P<φ<Γ>, φ<Γ>> φΓ_()
{
  slet a = φd<Γ>("Γ",
                 ":",  φauto(Γdef),
                 "$",  φauto(Γref),
                 ":/", φauto(Γvs),
                 "$/", φauto(Γvg),
                 "=",  φauto(Γpush),
                 "_",  φauto(Γdrop),
                 "??", φauto(Γdebug));
  slet r = Γφ(a, φΨ0(), φΨ1(), φΨ2(), φΨ4());
  return r;
}

φ<Γ> φΓa() { return φΓ_().first; }
φ<Γ> φΓ () { return φΓ_().second; }


Γ Γdef(σident n, Γ g)
{ return new Γf_{":" + n.n, [=](Ξc &x) { return x.gs(n.n, g); }}; }

Γ Γref(σident n)
{ return new Γf_{"$" + n.n, [=](Ξc &x) { return Γ{x.gg(n.n)}(x); }}; }


Γ Γdrop() { return new Γf_{"_", [](Ξc &x) { return x.drop(); }}; }
Γ Γpush() { return new Γf_{"=", [](Ξc &x) { return x.push({{}, {{}}}); }}; }

Γ Γswap()
{
  return new Γf_{"$$", [](Ξc &x)
    { let [a, y] = x.pop();
      let [b, z] = y.pop();
      return z.push(a).push(b); }};
}


Γ Γvs(σident n)
{
  return new Γf_{":/" + n.n, [n](Ξc &x)
    { let [a, y] = x.pop(); return y.vs(n.n, a); }};
}

Γ Γvg(σident n)
{
  return new Γf_{"$/" + n.n, [n](Ξc &x)
    { return x.push(x.vg(n.n)); }};
}


Γ Γdebug(σident tag)
{
  return new Γf_{"debug[" + tag.n + "]", [tag](Ξc &x)
    { std::cout << "debug " << tag.n << ": " << x << std::endl;
      return x; }};
}


}

#include "end.hh"
