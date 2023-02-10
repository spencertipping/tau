#include <iostream>
#include "../tau.hh"
#include "../tau/begin.hh"


using namespace τ;
using namespace std;


γ sum()
{ return γffn("∑", [](Sp<ψ> q, ξi i, ξo o)
  { u64 t = 0;
    for (let x : i) t += x.pu();
    o << η1o(t); }); }


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
        let x = *i;
        t = x.pu();
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

  auto g = γι(10)
    | γdrop(8)
    | γmap([](η i) { η0o r(η0t::tuple); r << η1o("foo") << i << i; return r; })
    | γflex(γϊ())
    | γflex(γτmap([](η i) { return η1o(Ss{} << i.pu() + 1); }))
    | γostream(cout);

  cout << "try_flex pipeline = " << g << endl;

  g(T).go();
  A(ψn() == 0, "leftover ψs: " << ψn());
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
