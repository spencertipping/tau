#include "Xi.hh"
#include "begin.hh"

namespace τ
{


P<ξo, ξi> Ξ::pipe(Sp<ψ> f, Sp<ψ> t, uN c)
{
  Sp<ξ> p(new ξ(l(), c));
  p->iq(f).oq(t);
  return mp(ξo(p), ξi(p));
}


Sp<ψ> Ξ::q(St name)
{
  Sp<ψ> r(new ψ(t()));
  r->name(name);
  return r;
}


O &operator<<(O &s, Ξ const &x)
{
  s << "Ξ" << x.a_.size() << "(";
  bool first = true;
  for (let &[k, _] : x.n_)
  { if (first) first = false;
    else       s << " ";
    s << k; }
  s << ")[";
  first = true;
  for (let &[k, _] : x.x_)
  { if (first) first = false;
    else       s << " ";
    s << k; }
  return s << "]";
}


}

#include "end.hh"
