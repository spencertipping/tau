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
    auto i = X.i("foo", nullptr, 16);
    auto o = X.o("foo", nullptr, 16);
    o.close();
    A(i.eof(), "!ieof");
    i.close();
  }

  {
    auto i = X.i("bar", nullptr, 32);
    auto o = X.o("bar", nullptr, 32);
    let &c = X["bar"].inner_ξ();

    A(o << η0o(57.5), "ξ<<");
    A(o << η0o(58.5), "ξ<<");
    A(o << η0o(59.5), "ξ<<");

    A(!(o <<= (η0o{}.t(η0t::utf8) << "01234567890123456789" << "01234567890123456789")),
      "ξ<<big didn't block");

    let x1 = *i; A(Sc<f64>(η1pi{x1}) == 57.5, "x1 " << η1pi{x1}); ++i;
    let x2 = *i; A(Sc<f64>(η1pi{x2}) == 58.5, "x2 " << η1pi{x2}); ++i;
    let x3 = *i; A(Sc<f64>(η1pi{x3}) == 59.5, "x3 " << η1pi{x3}); ++i;

    A(!c.extra(), "ξ extra nonzero");
    A(o << (η0o{}.t(η0t::utf8) << "01234567890123456789" << "01234567890123456789"),
      "ξ<<big");
    A(c.extra(), "ξ has no extra");

    let x4 = *i; A(x4.stv() == "0123456789012345678901234567890123456789",
                   "bogus string: " << x4.stv());
    ++i;

    o.close();
    A(i.eof(), "!ieof");
  }
}


void try_ints()
{
  Λ L;
  u64  j = 0;
  u64  c = 0;
  u64c N = 1048576 * 16;
  u64c K = 65536;

  {
    Ξ X(L);

    {
      let i = X.i("foo", nullptr, K);
      L.c([=, &j, &c]() {
        u64 t = 0;
        for (let x : i) ++c, t += Sc<u64>(η1pi{x});
        j = t;
      });
    }

    {
      auto o = X.o("foo", nullptr, K);
      L.c([=]() mutable {
        for (u64 i = 0; i < N; ++i)
          A(o << η0o{i}, "failed to write " << i);
        o.close();
      });
    }
  }

  let t0 = now();
  L.go();
  let t1 = now();

  cout << "ξ/" << K << " " << N << " ints in " << t1 - t0 << endl;

  A(c == N, "c = " << c << " (expected " << N << ")");
  A(j == N * (N - 1) / 2,
    "got " << j << ", expected " << N * (N - 1) / 2);
}


void try_big()
{
  Λ L;
  uN j = 0;
  uN k = 0;

  {
    Ξ X(L);
    {
      let i = X.i("foo", nullptr, 64);
      L.c([=, &j]() {
        for (let x : i)
          for (let y : η1ti{x})
            j += Sc<u64>(η1pi{y});
      });
    }

    {
      auto o = X.o("foo", nullptr, 64);
      L.c([=, &k]() mutable {
        for (u64 i = 0; i < 10000; ++i)
        {
          η0o x;
          x.t(η0t::tuple);
          for (u64 j = 0; j < i % 100; ++j)
            x << j, k += j;
          A(o << x, "write failed: " << x);
        }
      });
    }
  }

  L.go();
  A(j == k, "some values were lost or repeated; sent " << k << ", received " << j);
  cout << "try_big ok" << endl;
}


int main()
{
  τassert_begin;
  try_basic();
  try_ints();
  try_big();
  return 0;
  τassert_end;
}


#include "../tau/end.hh"
