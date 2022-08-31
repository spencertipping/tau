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
  γ g(l);  // just so we have one

  uN xs[] = {1, 2, 3, 5, 8, 13, 21, 34};
  auto ys = o9t("foo", 1, 2.0 + 3.0i, 3, 4.5,
                "bar", 6.7f, "abc",
                u9ω, u9τ, u9θ(0.5),
                5.0 + 8.9i, "bif");

  // Small test
  ζ t(l);
  let s1 = l.c([&]() {
    A(t.w(o9(xs, 8)), "ζ rejected xs");
    A(t.w(o9("foo")), "ζ rejected foo");
    A(t.w(ys), "ζ rejected ys");
    A(t.w(ys), "ζ rejected second ys");
    A(t.w(o9box(ys)), "ζ rejected boxed ys");
    t.wω();
    cout << "t write-closed" << endl;
    return 0;
  });

  let s2 = l.c([&]() {
    cout << "received " << i9{t.r<i9>()} << endl;
    cout << "received " << i9{t.r<i9>()} << endl;
    cout << "received " << i9{t.r<i9>()} << endl;
    cout << "received " << i9{t.r<i9>()} << endl;

    i9 b = t.r<i9>();
    cout << "received " << b << " = " << *b << endl;
    b.free();

    let z = t.r<i9>();
    A(z == ζωp, "z should be ζωp, got " << z);
    cout << "t is all good" << endl;
    return 0;
  });

  l.go();
  A(l.w(s1) == 0, "nonzero return from s1");
  A(l.w(s2) == 0, "nonzero return from s2");
}


void try_tuple()
{
  Λ l;
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

  a(b);

  l.go();
  A(!l.w(s1), "nonzero return from s1");
  A(!l.w(s2), "nonzero return from s2");
}


template<class F>
void bench(int argc)
{
  Λ l;
  φ<i9, F> a(l);
  φ<i9, F> b(l);

  uNc N = argc == 1
    ? std::is_same<F, φι>::value ? 1 << 28 : 1 << 24
    : 1 << 16;

  let f1 = l.c([&]() {
    for (u32 i = 0; i < N; ++i) A(a << o9(i), "φ rejected " << i);
    a.wω();
    for (i9 x : a) cout << "f1 received " << x << endl;
    return 0;
  });

  let f2 = l.c([&]() {
    u64 t = 0;
    uN  n = 0;
    for (i9 x : b) t += Sc<u32>(x), ++n;
    b.rω();
    A(n == N, "f2 got " << n << " (expected " << N << ")");
    cout << "f2 sending " << t << endl;
    b << o9(t);
    b.wω();
    return 0;
  });

  a(b);

  Θp t1 = now();
  l.go();
  Θp t2 = now();

  if (N >= 1 << 24)
    cout << "summing ints: " << (t2 - t1) / (N / (1 << 24)) << "/16M" << endl;

  if constexpr (std::is_same<F, φc>::value)
  {
    cout << "ao total: " << a.f.om << ", ohc = " << a.f.ohc << endl;
    cout << "bo total: " << b.f.om << ", ohc = " << b.f.ohc << endl;
  }

  if (!l.wi(f1)) cout << "f1 is not yet done" << endl;
  if (!l.wi(f2)) cout << "f2 is not yet done" << endl;
  cout << "f1 returning " << l.w(f1) << endl;
  cout << "f2 returning " << l.w(f2) << endl;
}


int main(int argc, char **argv)
{
  try_small_ζ();
  try_tuple();
  cout << "identity bench"      << endl; bench<φι>(argc);
  cout << "checked  bench"      << endl; bench<φc>(argc);
  cout << "measured bench (-H)" << endl; bench<φπ<ΘΔ>> (argc);
  cout << "measured bench (+H)" << endl; bench<φπ<ΣΘΔ>>(argc);
  return 0;
}

#include "../tau/end.hh"
