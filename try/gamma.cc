#include <iostream>
#include "../tau.hh"

using namespace τ;
using namespace std;


#include "../tau/begin.hh"


Γ iota(Ψd d = Ψd::f)
{
  return new ΓΨ1([](ψ, ξo o, Ψaux)
    { for (i64 i = 1;; ++i) if (o) o.r(12) << i; else break; }, d, "ι");
}

Γ take(i64 n, Ψd d = Ψd::f)
{
  return new ΓΨ2([n](ψ, ξi i, ξo o, Ψaux) mutable
    { for (let x : i)
        if (n-- <= 0) break;
        else if (o)   o.r(x.osize()) << x.outer();
      i.close(); },
    d, (Ss{} << "↑" << n).str());
}

Γ sum(Ψd d = Ψd::f)
{
  return new ΓΨ2([](ψ, ξi i, ξo o, Ψaux)
    { i64 t = 0;
      for (let x : i) if (o) o.r(12) << (t += x.i()); },
    d, "∑");
}

Γ last(Ψd d = Ψd::f)
{
  return new ΓΨ2([](ψ, ξi i, ξo o, Ψaux)
    { i64 y;
      for (let x : i) y = x.i();
      if (o) o.r(12) << y; },
    d, "↓₁");
}

Γ print(Ψd d = Ψd::f)
{
  return new ΓΨ0([](ψ, ξi i, Ψaux)
    { for (let x : i)
      { for (let y : x) cout << y << (y.has_next() ? " " : "");
        cout << endl; } },
    d, "out");
}


void try_iota()
{
  τe t;
  ( ΞΓpush() |
    iota() |
    take(10) |
    sum() |
    last() |
    print() )(Ξ{t});
  t.go();
  A(!ξn(), "ξs outlived try_iota: " << ξn());
  A(!ψn(), "ψs outlived try_iota: " << ψn());
}


void try_iota_rev()
{
  τe t;
  ( ΞΓpush() |
    print(Ψd::b) |
    last(Ψd::b) |
    sum(Ψd::b) |
    take(1000, Ψd::b) |
    iota(Ψd::b) )(Ξ{t});
  t.go();
  A(!ξn(), "ξs outlived try_iota_rev: " << ξn());
  A(!ψn(), "ψs outlived try_iota_rev: " << ψn());
}


void try_iota_loop()
{
  τe t;
  ( ΞΓpush() |
    print(Ψd::b) |
    last(Ψd::b) |
    iota(Ψd::f) |
    take(10000, Ψd::f) |
    sum(Ψd::r) )(Ξ{t});
  t.go();
  A(!ξn(), "ξs outlived try_iota_loop: " << ξn());
  A(!ψn(), "ψs outlived try_iota_loop: " << ψn());
}


int main()
{
  try_iota();
  try_iota_rev();
  try_iota_loop();
  return 0;
}


#include "../tau/end.hh"
