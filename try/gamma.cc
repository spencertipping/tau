#include <iostream>
#include "../tau.hh"

using namespace τ;
using namespace std;


#include "../tau/begin.hh"


Γ iota(Ψd d = Ψd::f)
{
  return new ΓΨ1([](ψ, ξo o, Ψaux)
    { for (i64 i = 1;; ++i) o.r(12) << i; }, d, "ι");
}

Γ weaken_eager()
{
  return new Γf_("weaken_eager", [](Ξc &x)
    { x.f().weaken();
      return x; });
}

Γ weaken(Ψd d = Ψd::f)
{
  return new ΓΨ2([](ψ, ξi i, ξo o, Ψaux)
    { i.weaken();
      for (let x : i) o.r(x.osize()) << x.outer(); },
    d, "weaken");
}

Γ take(i64 n, Ψd d = Ψd::f)
{
  return new ΓΨ2([n](ψ, ξi i, ξo o, Ψaux)
    { i64 e = 0;
      if (n)
        for (let x : i)
        { o.r(x.osize()) << x.outer();
          if (++e >= n) break; } },
    d, (Ss{} << "↑" << n).str());
}

Γ sum(Ψd d = Ψd::f)
{
  return new ΓΨ2([](ψ, ξi i, ξo o, Ψaux)
    { i64 t = 0;
      for (let x : i) o.r(12) << (t += x.i()); },
    d, "∑");
}

Γ last(Ψd d = Ψd::f)
{
  return new ΓΨ2([](ψ, ξi i, ξo o, Ψaux)
    { i64 y = 0;
      for (let x : i) y = x.i();
      o.r(12) << y; },
    d, "↓₁");
}

Γ debug(St prefix, Ψd d = Ψd::f)
{
  return new ΓΨ2([prefix](ψ, ξi i, ξo o, Ψaux)
    { for (let x : i)
      { cout << prefix << ": ";
        for (let y : x) cout << y << (y.has_next() ? " " : "");
        cout << endl;
        o.r(x.osize()) << x.outer(); } },
    d, "debug:" + prefix);
}

Γ print(Ψd d = Ψd::f)
{
  return new ΓΨ0([](ψ, ξi i, Ψaux)
    { for (let x : i)
      { for (let y : x) cout << y << (y.has_next() ? " " : "");
        cout << endl; } },
    d, "out");
}


Γ forever_server(St p)
{
  return new ΓΨ1([p](ψ q, ξo o, Ψaux)
    { q.b(p, [o](ψ &&q, Ξc &x)
      { let i = x.f();
        q.f([i, o]()
          { for (let x : i) o.r(x.osize()) << x.outer(); });
        return x.fx(); }); },
    Ψd::f, "forever_server:" + p, true);
}

Γ server(St p)
{
  return new ΓΨ2([p](ψ q, ξi i, ξo o, Ψaux)
    { q.b(p, [o](ψ &&q, Ξc &x)
      { let i = x.f();
        q.f([i, o]() { for (let x : i) o.r(x.osize()) << x.outer(); });
        return x.fx(); });

      // Provide a way to decommission the server, which should terminate
      // the ψ as soon as all client connections are done
      q.f([q, p, i]() mutable
        { for (let a : i)
            if (a.is_sig() && a.sig() == ηsig::ω)
              q.bx(p); }); },
    Ψd::f, "server:" + p, true);
}

Γ connect(St port)
{
  return new Γf_{"connect:" + port, [port](Ξc &x)
    { return x.c(port); }};
}


void try_gc1()
{
  τe t;
  // weaken_eager() should immediately free iota()
  ( ΞΓpush() | iota() | weaken_eager() | sum() | last() | print() )(Ξ{t});
  t.go();
  A(!ξn(), "ξs outlived try_gc: " << ξn());
  A(!ψn(), "ψs outlived try_gc: " << ψn());
}

void try_gc2()
{
  τe t;
  // weaken() should immediately free iota()
  ( ΞΓpush() | iota() | weaken() | sum() | last() | print() )(Ξ{t});
  t.go();
  A(!ξn(), "ξs outlived try_gc: " << ξn());
  A(!ψn(), "ψs outlived try_gc: " << ψn());
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


void try_server_simple()
{
  {
    τe t;
    ( ΞΓpush() | forever_server("p")
      | sum() | take(20) | last() | print() | ΞΓdrop()

      | ΞΓpush() | iota() | take(10) | connect("p") | ΞΓdrop()
      | ΞΓpush() | iota() | take(10) | connect("p") | ΞΓdrop() )(Ξ{t});
    t.go();

    A(!ξn(), "ξs outlived try_server_simple: " << ξn());
    A(ψn() == 1, "more than one ψ outlived try_server_simple: " << ψn());
  }
  A(!ψn(), "ψ outlived τ");
}


int main()
{
  try_gc1();
  try_gc2();
  try_iota();
  try_iota_rev();
  try_iota_loop();
  try_server_simple();
  return 0;
}


#include "../tau/end.hh"
