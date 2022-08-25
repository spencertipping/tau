#define tau_debug 1


#include <iostream>

#include "../tau.hh"


using namespace tau;
using namespace std;


#include "../tau/begin.hh"

int main()
{
  Λ l;
  ζ<i9, o9n<uN>> z(l);

  let f1 = l.c([&]() {
    for (uN i = 0; i < 10; ++i)
    {
      cout << "writing " << i << endl;
      if (!(z << o9n<uN>{i}))
      {
        cout << "ζ rejected " << i << endl;
        z.wω();
        return 1;
      }
      cout << z << endl;
    }
    cout << "wω ζ" << endl;
    z.wω();
    cout << z << endl;
    return 0;
  });

  let f2 = l.c([&]() {
    for (let &x : z)
    {
      cout << "received " << R<uN>(x.start(), 1) << endl;
      cout << z << endl;
    }
    return 0;
  });

  cout << "initial ζ: " << z << endl;

  cout << "starting coroutines" << endl;
  l.go();

  if (!l.wi(f1)) cout << "f1 is not yet done" << endl;
  if (!l.wi(f2)) cout << "f2 is not yet done" << endl;
  cout << "f1 returning " << l.w(f1) << endl;
  cout << "f2 returning " << l.w(f2) << endl;
  return 0;
}

#include "../tau/end.hh"
