#include <iostream>
#include "../tau.hh"

#include "../tau/begin.hh"

using namespace std;
using namespace τ;


void xi_bench()
{
  τe t;
  let [w, r] = t.pipe(16384);
  i64  s = 0;
  bool done = false;

  t.l().c([&, w=w]() {
    for (i64 i = 0; !done; ++i)
      if (!w) return;
      else    w.r(12) << i;
    w.close();
  });

  t.l().c([&, r=r]() {
    for (let x : r) s = x.i();
    cout << "ξ got final element: " << s << endl;
  });

  t.l().c([&]() {
    t.Θ(now() + 1s);
    done = true;
    cout << "set done = true" << endl;
  });

  let t1 = now();
  t.go();
  let t2 = now();

  cout << (t2 - t1) / s << " per element" << endl;
  cout << "ξ bench ok" << endl;
}


void xi_keys_bench()
{
  τe t;
  let [w, r] = t.pipe(16384);
  i64  s = 0;
  bool done = false;

  t.l().c([&, w=w]() {
    for (i64 i = 0; !done; ++i)
      if (!w) return;
      else    w.r() << ηname{"x"} << 1 << ηname{"y"} << i;
    w.close();
  });

  t.l().c([&, r=r]() {
    for (let x : r) s = x[ηname{"y"}].i();
    cout << "ξ keys got final element: " << s << endl;
  });

  t.l().c([&]() {
    t.Θ(now() + 1s);
    done = true;
    cout << "set done = true" << endl;
  });

  let t1 = now();
  t.go();
  let t2 = now();

  cout << (t2 - t1) / s << " per element keyed" << endl;
  cout << "ξ keys ok" << endl;
}


int main()
{
  xi_bench();
  xi_keys_bench();
  return 0;
}


#include "../tau/end.hh"
