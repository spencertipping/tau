#include "platform.hh"
#include "begin.hh"

namespace σ
{

using namespace std::literals;

using τ::Ψ;
using τ::ηm;
using τ::i64;
using τ::St;


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
  g .def_p0("F", []()      { return Ψ([](ξi i) { for (let x : i) (void) x; }); })
    .def_p1("f", []()      { return Ψ([](ψ q, ξo o) { τe &t = q.t(); while (1) t.dt(τ::now() + 1s); }); })
    .def_p1("i", [](ηm x)  { return Ψ([x=mo(x)](ξo o) { o.r() << x.y(); }); })
    .def_p1("I", [](ηm x)  { return Ψ([x=mo(x)](ξo o) { while (o) o.r() << x.y(); }); })
    .def_p1("n", [](i64 x) { return Ψ([=](ξo o) { for (i64 i = 0; i < x; ++i) o.r() << i; }); })

    .def_p2("M?",  []()     { return Ψ([] (ξi i, ξo o) { for (let x : i) std::cout <<              x << std::endl, o << x; }); })
    .def_p2("M??", [](St n) { return Ψ([=](ξi i, ξo o) { for (let x : i) std::cout << n << ": " << x << std::endl, o << x; }); });
}


void πshared(πφ &p)
{

}


}

#include "end.hh"
