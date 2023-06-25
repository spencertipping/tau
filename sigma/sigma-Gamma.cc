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
  g .def_p0("F", [](             ξi i) { for (let x : i) (void) x; })

    .def_p1("f", [](        ψ q, ξo o) { τe &t = q.t(); while (1) t.dt(now() + 1s); })
    .def_p1("i", [](ηm  x,       ξo o) { o << x.y(); })
    .def_p1("I", [](ηm  x,       ξo o) { while (o) o << x.y(); })
    .def_p1("n", [](i64 x,       ξo o) { for (i64 i = 0; i < x; ++i) o.r() << i; })

    .def_p2("-",   [](           ξi i, ξo o) { for (let x : i) o << x; })
    .def_p2("M?",  [](           ξi i, ξo o) { for (let x : i) std::cout <<              x << std::endl, o << x; })
    .def_p2("M??", [](St n,      ξi i, ξo o) { for (let x : i) std::cout << n << ": " << x << std::endl, o << x; })
    .def_p2("p",   [](πst<π0> f, ξi i, ξo o) { πi p; for (let x : i) { p.push(x); f.x(p); o << p[p.pop()]; } })
    ;
}


void πshared(πφ &p)
{
  p .def_sa([](i64 x)              { return πauto((Ss{} << x).str(), [x]() { return x; }); })
    .def_sa([](φaL<'$'>, φident n) { return πf<1>{"$" + n.x, [=](πi &i) { return i.mget(n.x); }}; })

    .def_sp("+", [](πse<i64> x, i64 y) { return x.x + y; })
    .def_sp("-", [](πse<i64> x, i64 y) { return y - x.x; })
    .def_sp(">", [](πse<ηi>  x, ηi  y) { return x.x < y; })
    .def_sp("<", [](πse<ηi>  x, ηi  y) { return x.x > y; })

    .def_sp(">s", [](ηi x) { return (Ss{} << x << "\n").str(); })
    ;
}


}

#include "end.hh"
