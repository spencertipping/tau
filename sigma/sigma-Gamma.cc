#include "platform.hh"
#include "begin.hh"

namespace σ
{

using namespace std::literals;
using namespace τ;


Γφ σΓ()
{
  Γφ r;
  Γshared(r);
  Γnative(r);
  πshared(r.pi());
  πnative(r.pi());
  return r;
}


void Γshared(Γφ &g)
{
  g .def_g("=",  [](Ξc &x) { return x.push(); })
    .def_g("_",  [](Ξc &x) { return x.drop(); })
    .def_g(":",  [](φident n, Γ g, Ξc &x) { return x.gs(n.x, g); })
    .def_g("$",  [](φident n,      Ξc &x) { return Γ{x.gg(n.x)}(x); })
    .def_g(":/", [](φident n,      Ξc &x) { let [d, y] = x.pop(); return y.vs(n.x, d); })
    .def_g("$/", [](φident n,      Ξc &x) { return x.push(x.vg(n.x)); })
    .def_g("$$", [](               Ξc &x) { let [a, y] = x.pop(); let [b, z] = y.pop(); return z.push(a).push(b); })

    .def_p0("F", [](             ξi i) { for (let x : i) (void) x; })

    .def_p1("f", [](        ψ q, ξo o) { τe &t = q.t(); while (1) t.dt(now() + 1s); })
    .def_p1("i", [](ηm  x,       ξo o) { o.r(x.lsize()) << x.all(); })
    .def_p1("I", [](ηm  x,       ξo o) { while (o) o.r(x.lsize()) << x.all(); })
    .def_p1("n", [](i64 x,       ξo o) { for (i64 i = 0; i < x; ++i) o.r() << i; })

    .def_p2("-",   [](           ξi i, ξo o) { for (let x : i) o << x; })
    .def_p2("k",   [](           ξi i, ξo o) { for (let x : i) o << x, o.r(2) << ηsig::τ; })
    .def_p2("K",   [](           ξi i, ξo o) { for (let x : i) if (!x.is_sig()) o << x; })
    .def_p2("N",   [](           ξi i, ξo o) { ηl(i, o, [&](ηi x) { for (i64 n = 0; n < x.i(); ++n) o.r(12) << n; }); })
    .def_p2("M?",  [](           ξi i, ξo o) { for (let x : i) std::cout <<              x << std::endl, o << x; })
    .def_p2("M??", [](St n,      ξi i, ξo o) { for (let x : i) std::cout << n << ": " << x << std::endl, o << x; })
    .def_p2("p",   [](πpt<π0> f, ξi i, ξo o) { πi p(i, o); for (let x : i) { p.push(x); f.x(p); o << p.ypop(); } })
    .def_p2("r",   [](πpt<π0> f, ξi i, ξo o) { πi p(i, o); for (let x : i) { p.push(x); f.x(p); if (p.ypop().b()) o << x; } })
    ;
}


void πshared(πφ &p)
{
  p .def_sa([](i64 x)              { return πk(x); })
    .def_sa([](ηname n)            { return πk(n); })
    .def_sa([](φaL<'$'>, φident n) { return πf<1>{"$" + n.x, [=](πi &i) { return i.mg(n.x); }}; })

    .def_sp("+", [](πse<i64> x, i64 y) { return x.x + y; })
    .def_sp("-", [](πse<i64> x, i64 y) { return y - x.x; })
    .def_sp(">", [](πse<ηi>  x, ηi  y) { return x.x < y; })
    .def_sp("<", [](πse<ηi>  x, ηi  y) { return x.x > y; })

    .def_sp(">s", [](ηi x) { return (Ss{} << x << "\n").str(); })
    ;
}


}

#include "end.hh"
