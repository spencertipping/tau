// NOTE XXX: this file produces memory errors for reasons I don't entirely
// understand. It seems to happen within each test.
//
// At some point I'd like to fix this, but it doesn't happen in production
// sigma builds so it's not top priority.

#include <iostream>
#include "../tau.hh"
#include "../sigma.hh"

using namespace τ;
using namespace std;


#include "../tau/begin.hh"


σ::Γφ grammar()
{
  σ::Γφ r = σ::σΓ();
  r .def_p1("iota",   [](ψ, ξo o) { for (i64 i = 1;; ++i) o.r(12) << i; })
    .def_g ("weager", [](Ξc &x) { x.f().weaken(); return x; })
    .def_p2("weaken", [](ξi i, ξo o)
      { i.weaken();
        for (let x : i) o.r(x.osize()) << x.outer(); })
    .def_p2("take", [](i64 n, ξi i, ξo o)
      { i64 e = 0;
        if (n)
          for (let x : i)
          { o.r(x.osize()) << x.outer();
            if (++e >= n) break; } })
    .def_p2("sum", [](ψ, ξi i, ξo o, Ψaux)
      { i64 t = 0;
        for (let x : i) o.r(12) << (t += x.i()); })
    .def_p2("last", [](ψ, ξi i, ξo o, Ψaux)
      { i64 y = 0;
        for (let x : i) y = x.i();
        o.r(12) << y; })
    .def_p2("debug", [](St prefix, ψ, ξi i, ξo o, Ψaux)
      { for (let x : i)
        { cout << prefix << ": ";
          for (let y : x) cout << y << (y.has_next() ? " " : "");
          cout << endl;
          o.r(x.osize()) << x.outer(); } })
    .def_p0("print", [](ψ, ξi i, Ψaux)
      { for (let x : i)
        { for (let y : x) cout << y << (y.has_next() ? " " : "");
          cout << endl; } })
    .def_c1("fserver", [](St p, ψ q, ξo o, Ψaux)
      { q.b(p, [o](ψ &&q, Ξc &x)
        { let i = x.f();
          q.f([i, o]()
            { for (let x : i) o.r(x.osize()) << x.outer(); });
          return x.fx(); }); })
    .def_c2("server", [](St p, ψ q, ξi i, ξo o, Ψaux)
      { q.b(p, [o](ψ &&q, Ξc &x)
        { let i = x.f();
          q.f([i, o]() { for (let x : i) o.r(x.osize()) << x.outer(); });
          return x.fx(); });
        q.f([q, p, i]() mutable
          { for (let a : i)
              if (a.is_sig() && a.sig() == ηsig::ω)
                q.bx(p); }); })
    .def_g("connect", [](St port, Ξc &x) { return x.c(port); })
    .def_g("push", [](Ξc &x) { return x.push(); })
    .def_g("drop", [](Ξc &x) { return x.drop(); });

  return r;
}


void try_gc1()
{
  τe t;
  // weaken_eager() should immediately free iota()
  grammar()("push iota weager sum last print")(Ξ{t});
  t.go();
  A(!ξn(), "ξs outlived try_gc1: " << ξn());
  A(!ψn(), "ψs outlived try_gc1: " << ψn());
}

void try_gc2()
{
  τe t;
  // weaken() should immediately free iota()
  grammar()("push iota weaken sum last print")(Ξ{t});
  t.go();
  A(!ξn(), "ξs outlived try_gc2: " << ξn());
  A(!ψn(), "ψs outlived try_gc2: " << ψn());
}


void try_iota()
{
  τe t;
  grammar()("push iota take10 sum last print")(Ξ{t});
  t.go();
  A(!ξn(), "ξs outlived try_iota: " << ξn());
  A(!ψn(), "ψs outlived try_iota: " << ψn());
}


void try_iota_rev()
{
  τe t;
  grammar()("push \\[print last sum take1000 iota]")(Ξ{t});
  t.go();
  A(!ξn(), "ξs outlived try_iota_rev: " << ξn());
  A(!ψn(), "ψs outlived try_iota_rev: " << ψn());
}


void try_iota_loop()
{
  τe t;
  grammar()("push \\print\\last iota take10000 |sum")(Ξ{t});
  t.go();
  A(!ξn(), "ξs outlived try_iota_loop: " << ξn());
  A(!ψn(), "ψs outlived try_iota_loop: " << ψn());
}


void try_server_simple()
{
  {
    τe t;
    grammar()("push fserver\"p\" sum take20 last print drop "
              "push iota take10 connect\"p\" drop "
              "push iota take10 connect\"p\" drop")(Ξ{t});
    t.go();

    A(!ξn(), "ξs outlived try_server_simple: " << ξn());
    A(ψn() == 1, "more than one ψ outlived try_server_simple: " << ψn());
  }
  A(!ψn(), "ψ outlived τ");
}


int main()
{
  τassert_begin;
  try_gc1();
  try_gc2();
  try_iota();
  try_iota_rev();
  try_iota_loop();
  try_server_simple();
  return 0;
  τassert_end;
}


#include "../tau/end.hh"
