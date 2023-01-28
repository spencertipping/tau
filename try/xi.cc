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
    auto i = X.i("foo", nullptr, 64);
    auto o = X.o("foo", nullptr, 64);
    o.close();
    A(i.eof(), "!ieof");
    i.close();
  }

  {
    auto i = X.i("bar", nullptr, 64);
    auto o = X.o("bar", nullptr, 64);

    A(o << η0o(57.5), "ξ<<");
    A(o << η0o(58.5), "ξ<<");
    A(o << η0o(59.5), "ξ<<");

    let x1 = *i; A(Sc<f64>(η1pi{x1}) == 57.5, "x1 " << η1pi{x1}); ++i;
    let x2 = *i; A(Sc<f64>(η1pi{x2}) == 58.5, "x2 " << η1pi{x2}); ++i;
    let x3 = *i; A(Sc<f64>(η1pi{x3}) == 59.5, "x3 " << η1pi{x3}); ++i;

    o.close();
    A(i.eof(), "!ieof");
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
    let i = X.i("foo", nullptr, K);
    L.c([=, &j, &c]() {
      u64 t = 0;
      for (let x : i) ++c, t += Sc<u64>(η1pi{x});
      j = t;
    });
  }
  cout << X << endl;

  {
    let o = X.o("foo", nullptr, K);
    L.c([=]() {
      for (u64 i = 0; i < N; ++i)
        A(o << η0o{i}, "failed to write " << i);
      o.close();
    });
  }
  cout << X << endl;

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
