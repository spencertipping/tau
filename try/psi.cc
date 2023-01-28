#include <iostream>
#include "../tau.hh"
#include "../tau/begin.hh"


using namespace τ;
using namespace std;


static uN ψs = 0;


struct γf : public virtual γ
{
  γf(Stc &n_, F<void(ξi, ξo)> &&f_) : n(n_), f(std::move(f_)) {}


  St name() const { return n; }

  Ξ operator()(Ξ &x)
    { ++ψs;
      Sp<ψ> q{new ψ(x.l()), [](ψ*) { --ψs; }};
      q->name(n);
      Ξ     y = x;
      let i = y.i("main", q);
      let o = y.o("next", q);
      let g = f;
      q->def([=](ψ &q) { g(i, o); });
      return y.mv("next", "main"); }


protected:
  Stc             n;
  F<void(ξi, ξo)> f;
};



Sp<γf> iota()
{ return Sp<γf>(new γf("ι", [](ξi i, ξo o)
  { for (u64 i = 0; o << η0o(i); ++i); })); }

Sp<γf> take(u64 n)
{ return Sp<γf>(new γf("⌊", [=](ξi i, ξo o) mutable
  { for (let x : i)
      if (!n-- || !(o << x))
      { i.close(); o.close(); break; } })); }

Sp<γf> sum()
{ return Sp<γf>(new γf("∑", [](ξi i, ξo o)
  { u64 t = 0;
    for (let x : i) t += Sc<u64>(η1pi{x});
    o << η0o(t); })); }


void try_gamma()
{
  Λ   L;
  λi  li = 0;
  u64 t  = 0;

  ξi i;

  {
    Ξ Y(L);

    {
      auto g = iota() | take(100) | sum();
      cout << "Γ = " << g << endl;
      Ξ X(L);
      Y = g(X);
    }

    i = Y.i("main", nullptr);
    A(i.inner_ξ().iq(), "disconnected ξ::iq (inside block)");
    A(i.inner_ξ().iq()->name() == "∑",
      "bogus name (inside block): " << i.inner_ξ().iq()->name());

    li = L.c([=, &t]()
      { A(!i.eof(), "EOF before sum result");
        let x = η1pi{*i};
        t = Sc<u64>(x);
        cout << "try_gamma ∑ 0..99 = " << x << endl; });
  }

  A(i.inner_ξ().iq(), "disconnected ξ::iq (outside block)");
  A(i.inner_ξ().iq()->name() == "∑",
    "bogus name (outside block): " << i.inner_ξ().iq()->name());

  A(ψs == 3, "at start, ψs == " << ψs);
  L.go();
  A(ψs == 1, "after go, ψs == " << ψs);
  i.close();
  A(ψs == 0, "after close, ψs == " << ψs);

  // By now our λs should be deallocated; all functions should have
  // returned
  for (λi j = 0; j <= li; ++j)
    A(!L.e(j), "Λ.e(" << j << "), " << li << " λs total");

  A(t == 100 * 99 / 2, "expected " << (100 * 99 / 2) << ", got " << t);
}


int main()
{
  τassert_begin;
  try_gamma();
  return 0;
  τassert_end;
}


#include "../tau/end.hh"
