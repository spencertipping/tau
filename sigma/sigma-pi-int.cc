#include "sigma-Gamma-pieces.hh"
#include "begin.hh"

namespace σ
{

using namespace std::literals;
using namespace τ;


void πshared_int(πφ &p)
{
  p .def_spost("+",   [](πse<i64> y, i64 x) { return x + y.x; })
    .def_spost("-",   [](πse<i64> y, i64 x) { return x - y.x; })
    .def_spost("*",   [](πse<i64> y, i64 x) { return x * y.x; })
    .def_spost("/",   [](πse<i64> y, i64 x) { return x / y.x; })
    .def_spost("%",   [](πse<i64> y, i64 x) { return x % y.x; })
    .def_spost("<<",  [](πse<i64> y, i64 x) { return x << y.x; })
    .def_spost(">>",  [](πse<i64> y, i64 x) { return x >> y.x; })
    .def_spost(">>>", [](πse<i64> y, i64 x) { return i64(u64(x) >> y.x); })
    .def_spost("&",   [](πse<i64> y, i64 x) { return x & y.x; })
    .def_spost("|",   [](πse<i64> y, i64 x) { return x | y.x; })
    .def_spost("^",   [](πse<i64> y, i64 x) { return x ^ y.x; })

    .def_spre("~", [](i64 x) { return ~x; })
    .def_spre("_", [](i64 x) { return -x; })
    ;
}


}

#include "end.hh"
