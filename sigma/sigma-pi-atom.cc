#include "sigma-Gamma-pieces.hh"
#include "begin.hh"

namespace σ
{

using namespace std::literals;
using namespace τ;


void πshared_atom(πφ &p)
{
  p .def_sa("",   [](πP<f64>   x) { return x.x; })
    .def_sa("",   [](πP<i64>   x) { return x.x; })
    .def_sa("",   [](πP<i64>   x) { return x.x; })
    .def_sa("",   [](πP<St>    x) { return x.x; })
    .def_sa("",   [](πP<ηname> n) { return n.x; })
    .def_sa("t.", []()            { return 6.28318530717959; })
    .def_sa("x",  [](πi &i)       { return i.x(); })
    .def_sa("y",  [](πi &i)       { return i.y(); })
    .def_sa("()", []()            { return ηi();  })
    .def_sa("@_", []()            { return πhr{}; })

    .def_sa("α", []() { return ηsig::α; })
    .def_sa("κ", []() { return ηsig::κ; })
    .def_sa("ι", []() { return ηsig::ι; })
    .def_sa("τ", []() { return ηsig::τ; })
    .def_sa("ω", []() { return ηsig::ω; })
    .def_sa("Ω", []() { return ηsig::Ω; })
    .def_sa("ρ", []() { return ηsig::ρ; })
    .def_sa("θ", []() { return ηsig::θ; })

    .def_sa("Sa", []() { return ηsig::α; })  // ASCII equivalents
    .def_sa("Sk", []() { return ηsig::κ; })
    .def_sa("Si", []() { return ηsig::ι; })
    .def_sa("St", []() { return ηsig::τ; })
    .def_sa("Sw", []() { return ηsig::ω; })
    .def_sa("SW", []() { return ηsig::Ω; })
    .def_sa("Sr", []() { return ηsig::ρ; })
    .def_sa("Sh", []() { return ηsig::θ; })

    .def_sa("A", [](πi &i) { return i.i(i.x(), i[i.x()].one()); })
    .def_sa("B", [](πi &i) { return i.i(i.x(), i[i.x()][1].one()); })
    .def_sa("C", [](πi &i) { return i.i(i.x(), i[i.x()][2].one()); })
    .def_sa("D", [](πi &i) { return i.i(i.x(), i[i.x()][3].one()); })
    .def_sa("E", [](πi &i) { return i.i(i.x(), i[i.x()][4].one()); })
    .def_sa("F", [](πi &i) { return i.i(i.x(), i[i.x()][5].one()); })

    .def_sa("$", [](πi &i, πP<φident> n) { return i.mg(n.x.x); })
    .def_pe(":", [](πi &i, πP<φident> n, πpe<π1> y, πpe<π1> z)
      { y.x(i); i.ms(n.x.x, i.pop()); z.x(i); return i.pop(); })
    ;
}


}

#include "end.hh"
