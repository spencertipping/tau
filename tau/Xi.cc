#include "Xi.hh"
#include "psi.hh"

#include "begin.hh"

namespace τ
{


// NOTE: this function must be defined out-of-line because it contains
// forward references to ψ members
Ξ Ξ::c(Stc &port) const
{
  return e_.route(port)->connect(port, *this);
}


ξd Ξkg(Sp<Ξk> x, Stc &k)
{
  for (; x; x = x->n)
    if (x->k == k) return x->v;
  A(0, "Ξ missing key: " << k);
  τunreachable();
}

Sp<Ξk> Ξkc(Sp<Ξk> x, Stc &k, ξd const &v, uN s)
{
  return Sp<Ξk>{new Ξk{k, v, s, x}};
}

Sp<Ξk> Ξks(Sp<Ξk> x, Stc &k, ξd const &v, uN s)
{
  if (!Ξki(x, k)) return Ξkc(x, k, v, s);
  Sk<Sp<Ξk>> xs;
  for (; x; xs.push(x), x = x->n)
    if (x->k == k)
    {
      x = Ξkc(x->n, k, v, s);
      while (!xs.empty())
      {
        x = Ξkc(x, xs.top()->k, xs.top()->v, xs.top()->s);
        xs.pop();
      }
      return x;
    }

  A(0, "Ξks internal error");
  τunreachable();
}

bool Ξki(Sp<Ξk> x, Stc &k)
{
  for (; x; x = x->n) if (x->k == k) return true;
  return false;
}

Sp<Ξk> Ξkx(Sp<Ξk> x, Stc &k)
{
  if (!x)        return x;
  if (x->k == k) return x->n;
  return Sp<Ξk>(new Ξk{x->k, x->v, x->s, Ξkx(x->n, k)});
}

Sp<Ξk> Ξkp(Sp<Ξk> x, uN s)
{
  while (x && x->s > s) x = x->n;
  return x;
}


O &operator<<(O &s, Ξ const &x)
{
  s << "Ξ s=" << x.s_ << ", depth=" << x.depth() << std::endl;
  for (auto y = x.t_; y; y = y->n)
    s << "  "  << y->io << std::endl;

  for (auto k = x.m_; k; k = k->n)
    s << "  kv  " << k->k << " = " << k->v << " @ " << k->s << std::endl;

  for (auto v = x.v_; v; v = v->n)
    s << "  var " << v->k << " = " << v->v << " @ " << v->s << std::endl;

  return s;
}


}

#include "end.hh"
