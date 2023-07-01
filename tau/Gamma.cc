#include "../tau.hh"
#include "begin.hh"

namespace τ
{


Γ operator|(Γ a, Γ b)
{
  // Since we're a normal consed list, append if the left-hand side
  // is itself a list. That way all of our results are flat.
  if (let h = a.as<Γs_>())
    return h->t ? h->h | Γ(dpc<Γ_>(h->t) | b)
                : h->h | b;

  // If the right side is a list, then glue on directly; otherwise,
  // wrap it in a cons-nil to form it into a tail.
  if (let g = b.as<Γs_>()) return new Γs_(a, g);
  else                     return new Γs_(a, new Γs_(b, nullptr));
}


Γ Γs(Vc<Γ> &xs)
{
  if (xs.empty()) return Γ();
  Γ r = xs.back();
  for (iN i = xs.size() - 2; i >= 0; --i) r = xs[i] | r;
  return r;
}


O &operator<<(O &s, Γ const &g)
{
  return s << g.name();
}


}

#include "end.hh"
