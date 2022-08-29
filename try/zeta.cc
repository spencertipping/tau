#define tau_debug 1
//#define tau_debug_i9st 1
#include "../tau.hh"
#include "../tau/debug.hh"


using namespace tau;
using namespace std;


#include "../tau/begin.hh"


void try_small_ζ()
{
  Λ l;

  uN xs[] = {1, 2, 3, 5, 8, 13, 21, 34};
  let ys = o9t("foo", 1, 2.0 + 3.0i, 3, 4.5,
               "bar", 6.7f, "abc",
               u9ω, u9τ, u9θ(0.5),
               5.0 + 8.9i, "bif");

  // Small test
  ζ t(l);
  let s1 = l.c([&]() {
    A(t.w(o9(xs, 8)), "ζ rejected xs");
    A(t.w(o9("foo")), "ζ rejected foo");
    A(t.w(ys), "ζ rejected ys");
    t.wω();
    cout << "t write-closed" << endl;
    return 0;
  });

  let s2 = l.c([&]() {
    cout << "received " << i9{t.r<i9>()} << endl;
    cout << "received " << i9{t.r<i9>()} << endl;
    cout << "received " << i9{t.r<i9>()} << endl;
    let z = t.r<i9>();
    A(z == ζωp, "z should be ζωp, got " << z);
    cout << "t is all good" << endl;
    return 0;
  });

  l.go();
  A(l.w(s1) == 0, "");
  A(l.w(s2) == 0, "");
}


void try_tuple()
{
  Λ l;
  ζ i(l);
  ζ o(l);
  φ<i9> a(l);
  φ<i9> b(l);

  let s1 = l.c([&]() {
    V<uN> xs;
    for (uN i = 0; i < 10; ++i)
    {
      xs.push_back(i);
      a << o9(xs);
    }
    a.wω();
    return 0;
  });

  let s2 = l.c([&]() {
    for (i9 x : b) cout << x << endl;
    return 0;
  });

  a(i, o);
  b(o, i);
  l.go();
}


void bench()
{
  Λ l;
  ζ i(l);
  ζ o(l);
  φ<i9> a(l);
  φ<i9> b(l);

  uNc N = 1 << 28;

  let f1 = l.c([&]() {
    for (uN i = 0; i < N; ++i) A(a << o9(i), "φ rejected " << i);
    a.wω();
    for (i9 x : a) cout << "f1 received " << x << endl;
    return 0;
  });

  let f2 = l.c([&]() {
    i64 t = 0;
    uN  n = 0;
    for (i9 x : b) t += Sc<iN>(x), ++n;
    b.rω();
    A(n == N, "f2 got " << n << " (expected " << N << ")");
    cout << "f2 sending " << t << endl;
    b << o9(t);
    b.wω();
    return 0;
  });

  a(i, o);
  b(o, i);

  Θp t1 = now();
  l.go();
  Θp t2 = now();

  if (N >= 1 << 24)
    cout << "summing ints: " << (t2 - t1) / (N / (1 << 24)) << "/16M" << endl;

  if (!l.wi(f1)) cout << "f1 is not yet done" << endl;
  if (!l.wi(f2)) cout << "f2 is not yet done" << endl;
  cout << "f1 returning " << l.w(f1) << endl;
  cout << "f2 returning " << l.w(f2) << endl;
}


int main()
{
  try_small_ζ();
  try_tuple();
  bench();
  return 0;
}

#include "../tau/end.hh"
