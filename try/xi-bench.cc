#include <iostream>
#include "../tau.hh"

#include "../tau/begin.hh"

using namespace std;
using namespace τ;


void xi_keys_bench()
{
  τe t;
  let [w, r] = t.pipe(16384);
  i64  s = 0;
  letc N = 16ll << 20;

  t.l().c([&, w=w]() {
    for (i64 i = 0; i < N; ++i)
      if (!w) return;
      else    w.r().k("x").v(1).k("y").v(i);
    w.close();
  });

  t.l().c([&, r=r]() {
    for (let x : r) s += x["y"].i();
    cout << "ξ keys got total: " << s << endl;
  });

  let t1 = now();
  t.go();
  let t2 = now();

  cout << "summed 16M keyed ints in " << t2 - t1 << endl;

  A(s == N * (N - 1) / 2, "got " << s << ", wanted " << N * (N - 1) / 2);
  cout << "ξ keys ok" << endl;
}


void xi_bench()
{
  τe t;
  let [w, r] = t.pipe(16384);
  i64  s = 0;
  letc N = 16ll << 20;

  t.l().c([&, w=w]() {
    for (i64 i = 0; i < N; ++i)
      if (!w) return;
      else    w.r(12) << i;
    w.close();
  });

  t.l().c([&, r=r]() {
    for (let x : r) s += x.i();
    cout << "got total: " << s << endl;
  });

  let t1 = now();
  t.go();
  let t2 = now();

  cout << "summed 16M ints in " << t2 - t1 << endl;
  A(s == N * (N - 1) / 2, "got " << s);
  cout << "ξ bench ok" << endl;
}


void xi_fast_bench()
{
  τe t;
  let [w, r] = t.pipe(16384);
  i64  s = 0;
  letc N = 16ll << 20;

  t.l().c([&, w=w]() {
    for (i64 i = 0; i < N; ++i) w.r(12) << i;
    w.close();
  });

  t.l().c([&, r=r]() {
    for (let x : r) s += x.i();
    cout << "got total: " << s << endl;
  });

  let t1 = now();
  t.go();
  let t2 = now();

  cout << "summed 16M ints in " << t2 - t1 << endl;
  A(s == N * (N - 1) / 2, "got " << s);
  cout << "ξ fast bench ok" << endl;
}


int main()
{
  xi_fast_bench();
  xi_bench();
  xi_keys_bench();
  return 0;
}


#include "../tau/end.hh"
