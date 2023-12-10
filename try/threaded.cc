#include <iostream>
#include "../tau.hh"

#include "../tau/begin.hh"

using namespace std;
using namespace τ;


void try_basic_thread()
{
  τe t;
  let [w, r] = t.pipe(256);

  t.l().c([&, w=w]() {
    cout << "starting worker" << endl;
    let x = t([]()
      { std::this_thread::sleep_for(100ms);
        cout << "worker returning 42" << endl;
        return 42; });
    cout << "worker returned " << x << endl;
    w.r() << x;
    w.close();
  });

  t.l().c([&, r=r]() {
    let  t0 = now();
    auto lt = now();
    int  qs = 0;

    // Make sure the event loop isn't blocked during threading
    while (now() - t0 < 200ms)
    {
      A(now() - lt < 10ms, "unexpected quantum of " << now() - lt);
      lt = now();
      t.Θ(now() + 1ms);
      ++qs;
    }

    A(qs > 100, "quantum count too low: " << qs);

    for (let &x : r)
      cout << "got " << x << endl;
  });

  cout << "starting τ" << endl;
  t.go();
  cout << "τ returned" << endl;
}


int main()
{
  try_basic_thread();
  cout << "all ok" << endl;
  return 0;
}


#include "../tau/end.hh"
