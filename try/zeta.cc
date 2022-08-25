#define tau_debug 1


#include <iostream>

#include "../tau.hh"


using namespace tau;
using namespace std;


#include "../tau/begin.hh"

int main()
{
  Λ l;
  ζ z(l);

  let f1 = l.c([&]() {
    for (uN i = 0; i < 1 << 28; ++i)
    {
      if (!(z << o9n<uN>{i}))
      {
        cout << "ζ rejected " << i << endl;
        z.wω();
        return 1;
      }
    }
    cout << "wω ζ" << endl;
    z.wω();
    cout << z << endl;
    return 0;
  });

  let f2 = l.c([&]() {
    uN t = 0;

    // FIXME: begin/end can't use templates
    for (i9 x : z) t += R<uN>(x.start(), 1);
    cout << "f2 total: " << t << endl;
    return 0;
  });

  cout << "initial ζ: " << z << endl;
  Θp a = now();
  l.go();
  Θp b = now();
  cout << "summing ints: " << (b - a) / (1 << 4) << "/16M" << endl;

  if (!l.wi(f1)) cout << "f1 is not yet done" << endl;
  if (!l.wi(f2)) cout << "f2 is not yet done" << endl;
  cout << "f1 returning " << l.w(f1) << endl;
  cout << "f2 returning " << l.w(f2) << endl;
  return 0;
}

#include "../tau/end.hh"
