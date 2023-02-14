#include "gamma.hh"
#include "begin.hh"

namespace τ
{


γ operator|(γ a, γ b)
{
  if (let h = a.as<Γ_>())
    return h->t ? h->h | (γ(h->t) | b) : h->h | b;

  let g = b.as<Γ_>();
  return γ(new Γ_{a, g ? g : ms(Γ_{b, {nullptr}})});
}


O &operator<<(O &s, γ const &g)
{
  return s << g.name();
}


}

#include "end.hh"
