#include <iostream>
#include "../tau.hh"

#include "../tau/begin.hh"

using namespace std;
using namespace τ;


void try_spans()
{
  u8c       xs[] = { 0x01, 0x02, 0x03, 0x04 };
  Sn<u16bc> ys{Rc<u16bc*>(xs), 2};
  A(ys[0] == 0x0102, "expected 0x0102, got " << ys[0]);
  A(ys[1] == 0x0304, "expected 0x0304, got " << ys[1]);
  cout << "spans are ok" << endl;
}


void try_xi_simple()
{
  τe t;
  ξ  x(t.l(), 256);
  i64  s   = 0;
  bool l1e = false;
  bool l2e = false;

  t.l().c([&]() {
    for (i64 i = 0; i < 10000; ++i) ηo(x) << i;
    x.close();
    l1e = true;
  });

  t.l().c([&]() {
    for (let y : x) s += ηi(y).i();
    cout << "got total: " << s << endl;
    l2e = true;
  });

  t.go();

  A(s == 10000 * 9999 / 2, "got " << s);
  A(l1e && l2e, "lambdas didn't terminate correctly");
}


void xi_bench()
{
  τe t;
  ξ  x(t.l(), 16384);
  i64  s = 0;
  letc N = 16ll << 20;

  t.l().c([&]() {
    for (i64 i = 0; i < N; ++i) ηo(x) << i;
    x.close();
  });

  t.l().c([&]() {
    for (let y : x) s += ηi(y).i();
    cout << "got total: " << s << endl;
  });

  let t1 = now();
  t.go();
  let t2 = now();

  cout << "summed 16M ints in " << t2 - t1 << endl;
  A(s == N * (N - 1) / 2, "got " << s);
}


int main()
{
  try_spans();
  try_xi_simple();
  xi_bench();
  return 0;
}


#include "../tau/end.hh"
