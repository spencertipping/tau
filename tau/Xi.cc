#include "Xi.hh"
#include "Gamma.hh"
#include "psi.hh"

#include "begin.hh"

namespace τ
{


// NOTE: this function must be defined out-of-line because it contains
// forward references to ψ members
Ξ Ξ::c(Stc &port) const
{
  let q = e_.route(port);
  A(q, "no ψ binds " << port);
  return q->connect(port, *this);
}


Tt T Ξkg(Sp<Ξk<T>> x, Stc &k)
{
  for (; x; x = x->n)
    if (x->k == k) return x->v;
  A(0, "Ξ missing key: " << k);
  τunreachable();
}

Tt Sp<Ξk<T>> Ξkc(Sp<Ξk<T>> x, Stc &k, T const &v, uN s)
{
  return Sp<Ξk<T>>{new Ξk<T>{k, v, s, x}};
}

Tt Sp<Ξk<T>> Ξks(Sp<Ξk<T>> x, Stc &k, T const &v, uN s)
{
  if (!Ξki(x, k)) return Ξkc(x, k, v, s);
  Sk<Sp<Ξk<T>>> xs;
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

Tt bool Ξki(Sp<Ξk<T>> x, Stc &k)
{
  for (; x; x = x->n) if (x->k == k) return true;
  return false;
}

Tt Sp<Ξk<T>> Ξkx(Sp<Ξk<T>> x, Stc &k)
{
  if (!x)        return x;
  if (x->k == k) return x->n;
  return Sp<Ξk<T>>(new Ξk<T>{x->k, x->v, x->s, Ξkx(x->n, k)});
}

Tt Sp<Ξk<T>> Ξkp(Sp<Ξk<T>> x, uN s)
{
  while (x && x->s > s) x = x->n;
  return x;
}


template ξd         Ξkg<ξd>(Sp<Ξk<ξd>>, Stc&);
template Sp<Ξk<ξd>> Ξkc<ξd>(Sp<Ξk<ξd>>, Stc&, ξdc&, uN);
template Sp<Ξk<ξd>> Ξks<ξd>(Sp<Ξk<ξd>>, Stc&, ξdc&, uN);
template bool       Ξki<ξd>(Sp<Ξk<ξd>>, Stc&);
template Sp<Ξk<ξd>> Ξkx<ξd>(Sp<Ξk<ξd>>, Stc&);
template Sp<Ξk<ξd>> Ξkp<ξd>(Sp<Ξk<ξd>>, uN);

template Sp<Γ_>         Ξkg<Sp<Γ_>>(Sp<Ξk<Sp<Γ_>>>, Stc&);
template Sp<Ξk<Sp<Γ_>>> Ξkc<Sp<Γ_>>(Sp<Ξk<Sp<Γ_>>>, Stc&, Sp<Γ_> const&, uN);
template Sp<Ξk<Sp<Γ_>>> Ξks<Sp<Γ_>>(Sp<Ξk<Sp<Γ_>>>, Stc&, Sp<Γ_> const&, uN);
template bool           Ξki<Sp<Γ_>>(Sp<Ξk<Sp<Γ_>>>, Stc&);
template Sp<Ξk<Sp<Γ_>>> Ξkx<Sp<Γ_>>(Sp<Ξk<Sp<Γ_>>>, Stc&);
template Sp<Ξk<Sp<Γ_>>> Ξkp<Sp<Γ_>>(Sp<Ξk<Sp<Γ_>>>, uN);


M<St, ξd> Ξ::ktop() const
{
  M<St, ξd> r;
  for (auto x = m_; x && x->s == s_; x = x->n)
    r.emplace(x->k, x->v);
  return r;
}

M<St, ξd> Ξ::vtop() const
{
  M<St, ξd> r;
  for (auto x = v_; x && x->s == s_; x = x->n)
    r.emplace(x->k, x->v);
  return r;
}


O &operator<<(O &s, Ξ const &x)
{
  s << "Ξ s=" << x.s_ << ", depth=" << x.depth() << std::endl;
  for (auto y = x.t_; y; y = y->n)
    s << "  "  << y->io << std::endl;

  for (auto k = x.m_; k; k = k->n)
    s << "  Ξk " << k->k << "@" << k->s << " = " << k->v << std::endl;

  for (auto v = x.v_; v; v = v->n)
    s << "  Ξv " << v->k << "@" << v->s << " = " << v->v << std::endl;

  for (auto v = x.g_; v; v = v->n)
    s << "  Γv " << v->k << "@" << v->s << " = " << Γ{v->v} << std::endl;

  return s;
}


}

#include "end.hh"
