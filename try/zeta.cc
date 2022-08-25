#define tau_debug 1


#include <iostream>

#include "../tau.hh"


using namespace tau;
using namespace std;


#include "../tau/begin.hh"

int main()
{
  Λ l;
  ζ i(l);
  ζ o(l);
  φ<i9> a(l);
  φ<i9> b(l);

  let f1 = l.c([&]() {
    for (uN i = 0; i < 1 << 24; ++i)
    {
      if (!(a << o9n<uN>{i}))
      {
        cout << "φ rejected " << i << endl;
        a.ω();
        return 1;
      }
    }
    a.ω();
    return 0;
  });

  let f2 = l.c([&]() {
    uN t = 0;
    for (i9 x : b) t += R<uN>(x.start(), 1);
    cout << "f2 total: " << t << endl;
    return 0;
  });

  a(i, o);
  b(o, i);

  Θp t1 = now();
  l.go();
  Θp t2 = now();
  cout << "summing ints: " << t2 - t1 << "/16M" << endl;

  if (!l.wi(f1)) cout << "f1 is not yet done" << endl;
  if (!l.wi(f2)) cout << "f2 is not yet done" << endl;
  cout << "f1 returning " << l.w(f1) << endl;
  cout << "f2 returning " << l.w(f2) << endl;
  return 0;
}

#include "../tau/end.hh"
