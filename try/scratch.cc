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
    cout << "ξ single got total: " << s << endl;
    l2e = true;
  });

  t.go();

  A(s == 10000 * 9999 / 2, "got " << s);
  A(l1e && l2e, "lambdas didn't terminate correctly");
  cout << "ξ single ok" << endl;
}


void try_xi_multi()
{
  τe t;
  ξ  x(t.l(), 64);  // force some expansions
  i64 s0 = 0;
  i64 s  = 0;

  for (i64 i = 0; i < 100; ++i)
    for (i64 j = 0; j < i; ++j)
      s0 += j;

  t.l().c([&]() {
    for (i64 i = 0; i < 100; ++i)
    {
      ηo o(x, 8);  // force many re-allocations
      for (i64 j = 0; j < i; ++j) o << j;
    }
    x.close();
  });

  t.l().c([&]() {
    for (let y : x)
      for (let z : ηi(y))
        s += z.i();
    cout << "ξ multi got total: " << s << endl;
  });

  t.go();

  A(s == s0, "got " << s << ", wanted " << s0);
  cout << "ξ multi ok" << endl;
}


void xi_bench()
{
  τe t;
  ξ  x(t.l(), 16384);
  i64  s = 0;
  letc N = 16ll << 20;

  t.l().c([&]() {
    for (i64 i = 0; i < N; ++i) ηo(x, 12) << i;
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
  cout << "ξ bench ok" << endl;
}


int main()
{
  try_spans();
  try_xi_simple();
  try_xi_multi();
  xi_bench();
  return 0;
}


#include "../tau/end.hh"
