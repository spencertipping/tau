#include "sigma-Gamma-pieces.hh"
#include "begin.hh"

namespace σ
{

using namespace std::literals;
using namespace τ;


void πshared_atom(πφ &p)
{
  p .def_sl("",   [](πP<f64>   x) { return x.x; })
    .def_sl("",   [](πP<i64>   x) { return x.x; })
    .def_sl("",   [](πP<i64>   x) { return x.x; })
    .def_sl("",   [](πP<St>    x) { return x.x; })
    .def_sl("",   [](πP<ηname> n) { return n.x; })
    .def_sl("t.", []()            { return 6.28318530717959; })
    .def_sl("x",  [](πi &i)       { return i.x(); })
    .def_sl("y",  [](πi &i)       { return i.y(); })
    .def_sl("()", []()            { return ηi();  })
    .def_sl("@_", []()            { return πhr{}; })

    .def_sl("α", []() { return ηsig::α; })
    .def_sl("κ", []() { return ηsig::κ; })
    .def_sl("ι", []() { return ηsig::ι; })
    .def_sl("τ", []() { return ηsig::τ; })
    .def_sl("ω", []() { return ηsig::ω; })
    .def_sl("Ω", []() { return ηsig::Ω; })
    .def_sl("ρ", []() { return ηsig::ρ; })
    .def_sl("θ", []() { return ηsig::θ; })

    .def_sl("Sa", []() { return ηsig::α; })  // ASCII equivalents
    .def_sl("Sk", []() { return ηsig::κ; })
    .def_sl("Si", []() { return ηsig::ι; })
    .def_sl("St", []() { return ηsig::τ; })
    .def_sl("Sw", []() { return ηsig::ω; })
    .def_sl("SW", []() { return ηsig::Ω; })
    .def_sl("Sr", []() { return ηsig::ρ; })
    .def_sl("Sh", []() { return ηsig::θ; })

    .def_sl("A", [](πi &i) { return i.i(i.x(), i[i.x()].one()); })
    .def_sl("B", [](πi &i) { return i.i(i.x(), i[i.x()][1].one()); })
    .def_sl("C", [](πi &i) { return i.i(i.x(), i[i.x()][2].one()); })
    .def_sl("D", [](πi &i) { return i.i(i.x(), i[i.x()][3].one()); })
    .def_sl("E", [](πi &i) { return i.i(i.x(), i[i.x()][4].one()); })
    .def_sl("F", [](πi &i) { return i.i(i.x(), i[i.x()][5].one()); })

    .def_sl("$", [](πi &i, πP<φident> n) { return i.mg(n.x.x); })
    .def_p (":", [](πi &i, πP<φident> n, πp<π1> y, πp<π1> z)
      { y.x(i); i.ms(n.x.x, i.pop()); z.x(i); return i.pop(); })
    ;
}


}

#include "end.hh"
