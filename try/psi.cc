#include <iostream>
#include "../tau.hh"
#include "../tau/begin.hh"


using namespace τ;
using namespace std;


Sp<γ> sum()
{ return γffn("∑", [](Sp<ψ> q, ξi i, ξo o)
  { u64 t = 0;
    for (let x : i) t += Sc<u64>(η1pi{x});
    o << η0o(t); }); }


void try_gamma()
{
  τe  T;
  λi  li = 0;
  u64 t  = 0;

  ξi i;

  {
    Ξ X(T);
    {
      auto g = γι() | γtake(100) | sum();
      cout << X;
      g(X);
      cout << g << " = " << X << endl;
    }

    i = X.p().fi().ensure(64);
    A(i.inner_ξ()->iq(), "disconnected ξ::iq (inside block)");
    A(i.inner_ξ()->iq()->name() == "∑",
      "bogus name (inside block): " << i.inner_ξ()->iq()->name());

    li = T.l().c([=, &t]()
      { A(!i.eof(), "EOF before sum result");
        let x = η1pi{*i};
        t = Sc<u64>(x);
        cout << "try_gamma ∑ 0..99 = " << x << endl; });
  }

  A(i.inner_ξ()->iq(), "disconnected ξ::iq (outside block)");
  A(i.inner_ξ()->iq()->name() == "∑",
    "bogus name (outside block): " << i.inner_ξ()->iq()->name());

               A(ψn() == 3, "at start, ψs == " << ψn());
  T.go();      A(ψn() == 1, "after go, ψs == " << ψn());
  i.weaken();  A(ψn() == 0, "after weaken(), ψs == " << ψn());
  i.close();   A(ψn() == 0, "after close, ψs == " << ψn());

  // By now our λs should be deallocated; all functions should have
  // returned
  for (λi j = 0; j <= li; ++j)
    A(!T.l().e(j), "Λ.e(" << j << "), " << li << " λs total");

  A(t == 100 * 99 / 2, "expected " << (100 * 99 / 2) << ", got " << t);
}


void try_flex()
{
  τe T;
  Ξ X(T);
  (γonce(η0o(100))
   | γϊ()
   | γmap([](η0i i) { u64 n = η1pi{i}; return η0o{}.t(η0t::tuple) << n / 10 << i; })
   | γflex(γmap([](η0i i) { return η0o(Sc<u64>(η1pi{i}) + 1); }))
   | γostream(cout))(X);
  T.go();
}


int main()
{
  τassert_begin;
  try_gamma();
  try_flex();
  return 0;
  τassert_end;
}


#include "../tau/end.hh"
