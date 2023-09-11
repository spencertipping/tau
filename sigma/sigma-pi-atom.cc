#include "sigma-Gamma-pieces.hh"
#include "begin.hh"

namespace σ
{

using namespace std::literals;
using namespace τ;


void πshared_atom(πφ &p)
{
  p .def_sa([](f64 x)         { return πk(x); })
    .def_sa([](i64 x)         { return πk(x); })
    .def_sa([](i64 x)         { return πk(x); })
    .def_sa([](St  x)         { return πk(x); })
    .def_sa([](ηname n)       { return πk(n); })
    .def_sa([](φaL<'t', '.'>) { return πk(6.28318530717959); })
    .def_sa([](φaL<'x'>)      { return π1{"x",  [](πi &i) { i.push(i.x()); }}; })
    .def_sa([](φaL<'y'>)      { return π1{"y",  [](πi &i) { i.push(i.y()); }}; })
    .def_sa([](φaL<'(', ')'>) { return π1{"()", [](πi &i) { i.push(ηi());  }}; })
    .def_sa([](φaL<'@', '_'>) { return π1{"@_", [](πi &i) { i.push(πhr{});  }}; })

    .def_sa([](φaU<0x3b1>) { return πk(ηsig::α); })
    .def_sa([](φaU<0x3ba>) { return πk(ηsig::κ); })
    .def_sa([](φaU<0x3b9>) { return πk(ηsig::ι); })
    .def_sa([](φaU<0x3c4>) { return πk(ηsig::τ); })
    .def_sa([](φaU<0x3c9>) { return πk(ηsig::ω); })
    .def_sa([](φaU<0x3a9>) { return πk(ηsig::Ω); })
    .def_sa([](φaU<0x3c1>) { return πk(ηsig::ρ); })
    .def_sa([](φaU<0x3b8>) { return πk(ηsig::θ); })

    .def_sa([](φaL<'S', 'a'>) { return πk(ηsig::α); })  // ASCII equivalents
    .def_sa([](φaL<'S', 'k'>) { return πk(ηsig::κ); })
    .def_sa([](φaL<'S', 'i'>) { return πk(ηsig::ι); })
    .def_sa([](φaL<'S', 't'>) { return πk(ηsig::τ); })
    .def_sa([](φaL<'S', 'w'>) { return πk(ηsig::ω); })
    .def_sa([](φaL<'S', 'W'>) { return πk(ηsig::Ω); })
    .def_sa([](φaL<'S', 'r'>) { return πk(ηsig::ρ); })
    .def_sa([](φaL<'S', 'h'>) { return πk(ηsig::θ); })

    .def_sa([](φaL<'A'>) { return π1{"A", [](πi &i) { i.push(i.i(i.x(), i[i.x()].one())); }}; })
    .def_sa([](φaL<'B'>) { return π1{"B", [](πi &i) { i.push(i.i(i.x(), i[i.x()][1].one())); }}; })
    .def_sa([](φaL<'C'>) { return π1{"C", [](πi &i) { i.push(i.i(i.x(), i[i.x()][2].one())); }}; })
    .def_sa([](φaL<'D'>) { return π1{"D", [](πi &i) { i.push(i.i(i.x(), i[i.x()][3].one())); }}; })
    .def_sa([](φaL<'E'>) { return π1{"E", [](πi &i) { i.push(i.i(i.x(), i[i.x()][4].one())); }}; })
    .def_sa([](φaL<'F'>) { return π1{"F", [](πi &i) { i.push(i.i(i.x(), i[i.x()][5].one())); }}; })

    .def_sa([](φaL<'$'>, φident n)
      { return π1{"$" + n.x, [=](πi &i) { i.push(i.mg(n.x)); }}; })

    .def_pe([](φaL<':'>, φident n, πpe<π1> y, πpe<π1> z)
      { return π1{n.x + "=", [=](πi &i) { y.x(i); i.ms(n.x, i.pop()); z.x(i); }}; })
    ;
}


}

#include "end.hh"
