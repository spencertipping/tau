#include <iostream>
#include "../tau.hh"
#include "../tau/begin.hh"


using namespace τ;
using namespace std;


void try_basic()
{
  Λ L;
  Ξ X(L);

  {
    auto i = X.i("foo", 64);
    auto o = X.o("foo", 64);
    A(!i.lock()->ra(),       "ira");
    A( i.lock()->wa() == 64, "iwa " << i.lock()->wa());
    o.close();
    A(i.lock()->eof(), "!ieof");
    i.close();
    A(!i.lock(), "iξ");
  }

  {
    auto  i = X.i("bar", 64);
    auto  o = X.o("bar", 64);
    auto &c = *i.lock();

    A(c.wa() == 64, "cwa0 " << c.wa());
    A(c.ra() == 0,  "cra0 " << c.ra());
    A(o << η0o(57.5), "ξ<<");
    A(c.wa() == 64 - 9, "cwa1 " << c.wa() << " vs " << 64-9);
    A(c.ra() == 9,  "cra1 " << c.ra());
    A(o << η0o(58.5), "ξ<<");
    A(c.wa() == 64 - 18, "cwa2 " << c.wa() << " vs " << 64-18);
    A(c.ra() == 18, "cra2 " << c.ra());
    A(o << η0o(59.5), "ξ<<");
    A(c.wa() == 64 - 27, "cwa3 " << c.wa() << " vs " << 64-27);
    A(c.ra() == 27, "cra3 " << c.ra());

    let x1 = *c; A(Sc<f64>(η1pi{x1}) == 57.5, "x1 " << η1pi{x1}); c.next();
    let x2 = *c; A(Sc<f64>(η1pi{x2}) == 58.5, "x2 " << η1pi{x2}); c.next();
    let x3 = *c; A(Sc<f64>(η1pi{x3}) == 59.5, "x3 " << η1pi{x3}); c.next();

    A(c.wa() == 64, "cwa4 " << c.wa());
    A(c.ra() == 0, "cra4 " << c.ra());
  }
}


void try_ints()
{
  Λ L;
  Ξ X(L);
  uNc N = 1048576;
  uNc K = 1024;
  u64 j = 0;
  u64 c = 0;

  {
    let i = X.i("foo", K);
    L.c([=, &j, &c]() {
      u64 t = 0;
      for (let x : i) ++c, t += Sc<u64>(η1pi{x});
      j = t;
    });
  }

  {
    let o = X.o("foo", K);
    L.c([=]() {
      for (u64 i = 0; i < N; ++i)
        A(o << η0o{i}, "failed to write " << i);
      o.close();
    });
  }

  let t0 = now();
  L.go();
  let t1 = now();

  cout << "ξ/" << K << " " << N << " ints in " << t1 - t0 << endl;

  A(c == N, "c = " << c << " (expected " << N << ")");
  A(j == N * (N - 1) / 2,
    "got " << j << ", expected " << N * (N - 1) / 2);
}


int main()
{
  τassert_begin;
  try_basic();
  try_ints();
  return 0;
  τassert_end;
}


#include "../tau/end.hh"
