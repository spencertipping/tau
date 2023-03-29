#include "Gamma.hh"
#include "begin.hh"

namespace τ
{


Γ operator|(Γ a, Γ b)
{
  if (let h = a.as<Γs_>())
    return h->t ? h->h | (Γ(h->t) | b) : h->h | b;

  let g = b.as<Γ_>();
  return Γ(new Γs_{a, g ? g : Sp<Γs_>(new Γs_{b, {nullptr}})});
}


O &operator<<(O &s, Γ const &g)
{
  return s << g.name();
}


}

#include "end.hh"
