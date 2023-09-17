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
    for (let &[y] : r.every<i64>()) s += y;
    cout << "ξ single got total: " << s << endl;
    l2e = true;
  });

  t.go();

  A(s == 10000 * 9999 / 2, "got " << s);
  A(l1e && l2e, "lambdas didn't terminate correctly");
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
      for (let &y : ηi(x).every<i64>())
        s += y;
    cout << "ξ multi got total: " << s << endl;
  });

  t.go();

  A(s == s0, "got " << s << ", wanted " << s0);
}


void try_xi_head()
{
  τe t;
  i64  s   = 0;
  i64  u   = 0;
  bool l1e = false;
  bool l2e = false;

  letc debug = false;

  if (debug) cout << "starting try_xi_head" << endl;

  Wp<ξ> p;
  Wp<ξ> q;

  {
    let [w, r] = t.pipe(256);
    if (debug) cout << "after pipe: " << w.inner_ξ().use_count() - 1 << endl;
    p = r.inner_ξ();
    if (debug) cout << "after p capture: " << p.use_count() << endl;

    // Just make sure this doesn't bump the usage count (it shouldn't)
    q = r.inner_ξ();
    if (debug) cout << "after q capture: " << p.use_count() << endl;
    q.reset();

    t.l().c([&, w=w]() {
      if (debug) cout << "w start: " << p.use_count() << endl;
      for (i64 i = 0; i < 10000; ++i)
      {
        w.r() << i;  // sic: make sure w.r() << i is ok on closed ξ
        if (!w) break;
        u += i;
      }
      if (debug) cout << "before w close: " << p.use_count() << endl;
      w.close();
      if (debug) cout << "after w close: " << p.use_count() << endl;
      l1e = true;
    });

    if (debug) cout << "after writer lambda: " << p.use_count() << endl;

    t.l().c([&, r=r]() mutable {
      if (debug) cout << "r start: " << p.use_count() << endl;
      for (let x : r)
      {
        if (x.i() >= 1000)
        {
          A(ξn() == 1, "|ξ| > 1 ????");
          if (debug) cout << "before r close: " << p.use_count() << endl;
          r.close();
          if (debug) cout << "after r close: " << p.use_count() << ", inner_ξ = " << r.inner_ξ().get() << endl;
          break;
        }
        s += x.i();
      }

      if (debug) cout << "after loop: " << p.use_count() << endl;

      if (!debug) A(ξn() == 0, "unexpected ξs lingering after r.close: " << ξn() << ", Λ = " << t.l());
      cout << "ξ single got total: " << s << endl;
      l2e = true;
    });

    if (debug) cout << "after reader lambda: " << p.use_count() << endl;
  }

  if (debug) cout << "before t go: " << p.use_count() << endl;
  t.go();
  if (debug) cout << "after t go: " << p.use_count() << endl;

  A(!t.l().n(), "lingering λs: " << t.l());

  A(u < 10000 * 9999 / 2, "got " << u);
  A(l1e, "lambda 1 didn't terminate correctly");
  A(l2e, "lambda 2 didn't terminate correctly");
  A(!ξn(), "expected no ξs to live, got " << ξn());
}


int main()
{
  try_spans();
  try_xi_simple();
  try_xi_multi();
  try_xi_head();
  cout << "all ok" << endl;
  return 0;
}


#include "../tau/end.hh"
