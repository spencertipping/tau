#define τdebug 1
//#define τdebug_i9st 1
//#define τdebug_ζclear 1
//#define τdebug_ζfill  255
#include "../tau.hh"
#include "../tau/debug.hh"


using namespace τ;
using namespace std;


#include "../tau/begin.hh"


void try_small_ζ()
{
  Λ l;

  uN xs[] = {1, 2, 3, 5, 8, 13, 21, 34};
  auto ys = o9t("foo", "bar", "bif", 1, 2.0 + 3.0i, 3, 4.5,
                "bar", 6.7f, "abc",
                u9ω(), u9τ(5), u9θ(0.5, 2),
                5.0 + 8.9i, "bif");

  // Small test
  ζ<i9> t(l);
  let s1 = l.c([&]() {
    A(t << o9(xs, 8), "ζ rejected xs");
    A(t << o9("foo"), "ζ rejected foo");
    A(t << ys, "ζ rejected ys");
    A(t << ys, "ζ rejected second ys");
    A(t << o9box(ys), "ζ rejected boxed ys");
    A(t << o9t("foo", std::string("bar")), "ζ rejected thing");
    A(t << ys.m(), "ζ rejected .m()");
    A(t << ys.s(), "ζ rejected .s()");
    t.wω();
    cout << "t write-closed" << endl;
    return 0;
  });

  let s2 = l.c([&]() {
    cout << "received " << i9{*t} << endl; A(i9{*t}.verify() == i9v::valid, "verify() failed: " << i9{*t} << " = " << i9{*t}.verify()); ++t;
    cout << "received " << i9{*t} << endl; A(i9{*t}.verify() == i9v::valid, "verify() failed: " << i9{*t} << " = " << i9{*t}.verify()); ++t;
    cout << "received " << i9{*t} << endl; A(i9{*t}.verify() == i9v::valid, "verify() failed: " << i9{*t} << " = " << i9{*t}.verify()); ++t;
    cout << "received " << i9{*t} << endl; A(i9{*t}.verify() == i9v::valid, "verify() failed: " << i9{*t} << " = " << i9{*t}.verify()); ++t;

    i9 b = *t;
    cout << "received " << b << " = " << *b << endl;
    A(i9{*t}.verify() == i9v::valid, "verify() failed: " << i9{*t} << " = " << i9{*t}.verify());
    ++t;  // NOTE: this should free b

    cout << "received " << i9{*t} << endl;
    A(i9{*t}.verify() == i9v::valid, "verify() failed: " << i9{*t} << " = " << i9{*t}.verify());
    ++t;

    cout << "received " << i9{*t} << endl; A(i9{*t}.verify() == i9v::valid, "verify() failed: " << i9{*t} << " = " << i9{*t}.verify()); ++t;
    cout << "received " << i9{*t} << endl; A(i9{*t}.verify() == i9v::valid, "verify() failed: " << i9{*t} << " = " << i9{*t}.verify()); ++t;

    let z = *t;
    A(z == ζωp, "z should be ζωp, got " << z);
    ++t;

    cout << "t is all good" << endl;
    return 0;
  });

  l.go();

  A(!l.e(s1), "s1 still exists");
  A(!l.e(s2), "s2 still exists");
}


void try_tuple()
{
  Λ l;
  φ_<i9> a(l);
  φ_<i9> b(l);

  let s1 = l.c([&]() {
    V<uN> xs;
    for (uN i = 0; i < 10; ++i)
    {
      xs.push_back(i);
      a << o9(xs);
    }
    a.ω();
    return 0;
  });

  let s2 = l.c([&]() {
    for (i9 x : b)
    {
      A(x.verify() == i9v::valid, "verify() failed: " << x << " = " << x.verify());
      cout << x << endl;
    }
    return 0;
  });

  a(b);

  l.go();
  A(!l.e(s1), "s1 still exists");
  A(!l.e(s2), "s2 still exists");
}


void try_bytes()
{
  Λ l;
  φ_<i9> a(l);
  φ_<i9> b(l);

  let s1 = l.c([&]() {
    u8 bs[] = {65, 66, 67, 68, 97, 98, 99, 100};
    a << o9t(1, 2, 3, 4,            Bv{bs, sizeof(bs)});
    a << o9t("foo", "bar", true,    Bv{bs, 5});
    a << o9t(1.2, 3.4, "foo", 5, 6, Bv{bs, 1});
    a.ω();
    return 0;
  });

  let s2 = l.c([&]() {
    for (i9 x : b) cout << x << endl;
    return 0;
  });

  a(b);

  l.go();
  A(!l.e(s1), "s1 still exists");
  A(!l.e(s2), "s2 still exists");
}


int main(int argc, char **argv)
{
  τassert_begin;
  try_small_ζ();
  try_tuple();
  try_bytes();
  τassert_end;
  return 0;
}


#include "../tau/end.hh"
