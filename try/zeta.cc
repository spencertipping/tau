#define tau_debug 1
#include "../tau.hh"
#include "../tau/debug.hh"


using namespace tau;
using namespace std;


#include "../tau/begin.hh"


void try_small_ζ()
{
  Λ l;

  // Small test
  ζ t(l);
  let s1 = l.c([&]() {
    A(t.w(o9<uN>{17}), "ζ rejected 17");
    t.wω();
    cout << "t write-closed" << endl;
    return 0;
  });

  let s2 = l.c([&]() {
    let x = i9{t.r<i9>()}; A(Sc<uN>(x) == 17, "got a mystery value: " << x);
    let y = t.r<i9>();     A(y == ζωp, "y should be ζωp, got " << y);
    cout << "t is all good" << endl;
    return 0;
  });

  l.go();
  A(l.w(s1) == 0, "");
  A(l.w(s2) == 0, "");
}


int main()
{
  Λ l;
  ζ i(l);
  ζ o(l);
  φ<i9> a(l);
  φ<i9> b(l);

  uNc N = 1 << 28;

  try_small_ζ();


  let f1 = l.c([&]() {
    for (uN i = 0; i < N; ++i) A(a << o9<uN>{i}, "φ rejected " << i);
    a.wω();
    for (i9 x : a) cout << "f1 received " << Sc<uN>(x) << endl;
    return 0;
  });

  let f2 = l.c([&]() {
    uN t = 0;
    uN n = 0;
    for (i9 x : b) t += Sc<uN>(x), ++n;
    b.rω();
    A(n == N, "f2 got " << n << " (expected " << N << ")");
    cout << "f2 sending " << t << endl;
    b << o9<uN>{t};
    b.wω();
    return 0;
  });

  a(i, o);
  b(o, i);

  Θp t1 = now();
  l.go();
  Θp t2 = now();
  cout << "summing ints: " << (t2 - t1) / (N / (1 << 24)) << "/16M" << endl;

  if (!l.wi(f1)) cout << "f1 is not yet done" << endl;
  if (!l.wi(f2)) cout << "f2 is not yet done" << endl;
  cout << "f1 returning " << l.w(f1) << endl;
  cout << "f2 returning " << l.w(f2) << endl;
  return 0;
}

#include "../tau/end.hh"
