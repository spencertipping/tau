#include "platform.hh"
#include "quote.hh"
#include "sigma-Gamma-pieces.hh"
#include "begin.hh"

namespace σ
{

using namespace std::literals;
using namespace τ;


Γφ σΓ()
{
  Γφ r;
  Γshared(r);
  Γnative(r);
  πshared(r.pi());
  πnative(r.pi());
  return r;
}


void Γshared(Γφ &g)
{
  Γshared_ψ(g);
  Γquote(g);

  g .def_g("=",  [](Ξc &x) { return x.push(); })
    .def_g("_",  [](Ξc &x) { return x.drop(); })
    .def_g(":",  [](φident n, Γ g, Ξc &x) { return x.gs(n.x, g); })
    .def_g("$",  [](φident n,      Ξc &x) { return Γ{x.gg(n.x)}(x); })
    .def_g(":/", [](φident n,      Ξc &x) { let [d, y] = x.pop(); return y.vs(n.x, d); })
    .def_g("$/", [](φident n,      Ξc &x) { return x.push(x.vg(n.x)); })
    .def_g("$$", [](               Ξc &x) { let [a, y] = x.pop(); let [b, z] = y.pop(); return z.push(a).push(b); })
    ;
}


void πshared(πφ &p)
{
  πshared_atom(p);
  πshared_int(p);
  πshared_float(p);
  πshared_η(p);
  πshared_cmp(p);
  πshared_list(p);
  πshared_map(p);
  πshared_string(p);
}


}

#include "end.hh"
