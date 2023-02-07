#include <iostream>
#include "../tau.hh"
#include "../tau/begin.hh"


using namespace τ;
using namespace std;


void try_basic()
{
  τe T;
  Ξ X(T);

  {
    auto [_1, o] = X.xf(nullptr, 16, 16);
    auto [i, _2] = X.xf(nullptr, 16, 16);
    o.close();
    A(i.eof(), "!ieof");
    i.close();
  }

  {
    auto [_1, o] = X.xf(nullptr, 32, 32);
    auto [i, _2] = X.xf(nullptr, 32, 32);
    let c = i.inner_ξ();

    A(o << η1o(57.5), "ξ<<");
    A(o << η1o(58.5), "ξ<<");
    A(o << η1o(59.5), "ξ<<");

    η0o os(η0t::utf8);
    A(!(o <<= (os << "01234567890123456789" << "01234567890123456789")),
      "ξ<<big didn't block");

    let x1 = *i; A(x1.pf() == 57.5, "x1 " << x1); ++i;
    let x2 = *i; A(x2.pf() == 58.5, "x2 " << x2); ++i;
    let x3 = *i; A(x3.pf() == 59.5, "x3 " << x3); ++i;

    A(!c->extra(), "ξ extra nonzero");
    η0o ot(η0t::utf8);
    A(o << (ot << "01234567890123456789" << "01234567890123456789"),
      "ξ<<big");
    A(c->extra(), "ξ has no extra");

    let x4 = *i; A(x4.stv() == "0123456789012345678901234567890123456789",
                   "bogus string: " << x4.stv());
    ++i;

    o.close();
    A(i.eof(), "!ieof");
  }
}


void try_ints()
{
  τe T;
  u64  j = 0;
  u64  c = 0;
  u64c N = 1048576 * 16;
  u64c K = 65536;

  {
    Ξ X(T);

    {
      {
        auto [_1, o] = X.xf(nullptr, K, K);
        T.l().c([o=o]() mutable {
          for (u64 i = 0; i < N; ++i)
            A(o << η1o(i), "failed to write " << i);
          o.close();
        });
      }

      {
        auto [i, _2] = X.xf(nullptr, K, K);
        T.l().c([i=i, &j, &c]() {
          u64 t = 0;
          for (let x : i) ++c, t += x.pu();
          j = t;
        });
      }
    }

  }

  let t0 = now();
  T.go();
  let t1 = now();

  cout << "ξ/" << K << " " << N << " ints in " << t1 - t0 << endl;

  A(c == N, "c = " << c << " (expected " << N << ")");
  A(j == N * (N - 1) / 2,
    "got " << j << ", expected " << N * (N - 1) / 2);
}


void try_big()
{
  τe T;
  uN j = 0;
  uN k = 0;

  {
    Ξ X(T);
    {
      auto [_1, o] = X.xf(nullptr, 64, 64);
      T.l().c([o=o, &k]() mutable {
        for (u64 i = 0; i < 10000; ++i)
        {
          η0o x(η0t::tuple);
          for (u64 j = 0; j < i % 100; ++j)
            x << η1o(j), k += j;
          A(o << x, "write failed: " << x);
        }
      });
    }

    {
      auto [i, _2] = X.xf(nullptr, 64, 64);
      T.l().c([i=i, &j]() {
        for (let x : i)
          for (let y : x.T())
            j += y.pu();
      });
    }
  }

  T.go();
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
