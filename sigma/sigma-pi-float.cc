#include "sigma-Gamma-pieces.hh"
#include "begin.hh"

namespace σ
{

using namespace std::literals;
using namespace τ;


void πshared_float(πφ &p)
{
  p .def_sr("+.", [](f64 x, πs<f64> y) { return x + y.x; })
    .def_sr("-.", [](f64 x, πs<f64> y) { return x - y.x; })
    .def_sr("*.", [](f64 x, πs<f64> y) { return x * y.x; })
    .def_sr("/.", [](f64 x, πs<f64> y) { return x / y.x; })
    .def_sr("%.", [](f64 x, πs<f64> y) { return fmod(x, y.x); })

    .def_sl("|.", [](πs<f64> x) { return fabs(x.x); })
    .def_sl("_.", [](πs<f64> x) { return -x.x; })

    .def_sl("S.", [](πs<f64> x) { return sin(x.x); })
    .def_sl("C.", [](πs<f64> x) { return cos(x.x); })
    .def_sl("T.", [](πs<f64> x) { return tan(x.x); })
    .def_sl("L.", [](πs<f64> x) { return log(x.x); })
    .def_sl("E.", [](πs<f64> x) { return exp(x.x); })

    .def_sr("Θ.", [](πi &i, f64 x)
      { i.t()->Θ(now() + i64(x * 1e9) * 1ns);
        return x; })
    ;
}


}

#include "end.hh"
