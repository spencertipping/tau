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
  let [w, r] = t.pipe(256);
  i64  s   = 0;
  bool l1e = false;
  bool l2e = false;

  t.l().c([&, w=w]() {
    for (i64 i = 0; i < 10000; ++i)
      if (!w) return;
      else    w.r() << i;
    w.close();
    l1e = true;
  });

  t.l().c([&, r=r]() {
    for (let y : r) s += y.i();
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
  let [w, r] = t.pipe(64);  // force some expansions
  i64 s0 = 0;
  i64 s  = 0;

  for (i64 i = 0; i < 100; ++i)
    for (i64 j = 0; j < i; ++j)
      s0 += j;

  t.l().c([&, w=w]() {
    for (i64 i = 0; i < 100; ++i)
    {
      if (!w) return;
      ηo o = w.r(8);  // force many re-allocations
      for (i64 j = 0; j < i; ++j) o << j;
    }
    w.close();
  });

  t.l().c([&, r=r]() {
    for (let x : r)
      for (let y : ηi(x))
        s += y.i();
    cout << "ξ multi got total: " << s << endl;
  });

  t.go();

  A(s == s0, "got " << s << ", wanted " << s0);
  cout << "ξ multi ok" << endl;
}


void try_xi_head()
{
  τe t;
  i64  s   = 0;
  i64  u   = 0;
  bool l1e = false;
  bool l2e = false;

  {
    let [w, r] = t.pipe(256);
    t.l().c([&, w=w]() {
      for (i64 i = 0; i < 10000; ++i)
      {
        w.r() << i;  // sic: make sure w.r() << i is ok on closed ξ
        if (!w) break;
        u += i;
      }
      w.close();
      l1e = true;
    });

    t.l().c([&, r=r]() mutable {
      for (let x : r)
      {
        if (x.i() >= 1000)
        {
          A(ξn() == 1, "|ξ| > 1 ????");
          r.close();
          break;
        }
        s += x.i();
      }

      A(ξn() == 0, "unexpected ξs lingering after r.close: " << ξn());
      cout << "ξ single got total: " << s << endl;
      l2e = true;
    });
  }

  t.go();

  A(!t.l().n(), "lingering λs: " << t.l());

  A(u < 10000 * 9999 / 2, "got " << u);
  A(l1e, "lambda 1 didn't terminate correctly");
  A(l2e, "lambda 2 didn't terminate correctly");
  A(!ξn(), "expected no ξs to live, got " << ξn());
  cout << "ξ single ok" << endl;
}


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
  try_spans();
  try_xi_simple();
  try_xi_multi();
  try_xi_head();
  xi_fast_bench();
  xi_bench();
  xi_keys_bench();
  return 0;
}


#include "../tau/end.hh"
