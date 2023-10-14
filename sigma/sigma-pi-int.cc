#include "sigma-Gamma-pieces.hh"
#include "begin.hh"

namespace σ
{

using namespace std::literals;
using namespace τ;


void πshared_int(πφ &p)
{
  p .def_sr("+",   [](i64 x, πs<i64> y) { return x + y.x; })
    .def_sr("-",   [](i64 x, πs<i64> y) { return x - y.x; })
    .def_sr("*",   [](i64 x, πs<i64> y) { return x * y.x; })
    .def_sr("/",   [](i64 x, πs<i64> y) { return x / y.x; })
    .def_sr("%",   [](i64 x, πs<i64> y) { return x % y.x; })
    .def_sr("<<",  [](i64 x, πs<i64> y) { return x << y.x; })
    .def_sr(">>",  [](i64 x, πs<i64> y) { return x >> y.x; })
    .def_sr(">>>", [](i64 x, πs<i64> y) { return i64(u64(x) >> y.x); })
    .def_sr("&",   [](i64 x, πs<i64> y) { return x & y.x; })
    .def_sr("|",   [](i64 x, πs<i64> y) { return x | y.x; })
    .def_sr("^",   [](i64 x, πs<i64> y) { return x ^ y.x; })

    .def_sl("~", [](πsl<i64> x) { return ~x.x; })
    .def_sl("_", [](πsl<i64> x) { return -x.x; })
    ;
}


}

#include "end.hh"
