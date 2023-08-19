#include "sigma-Gamma-pieces.hh"
#include "begin.hh"

namespace σ
{

using namespace std::literals;
using namespace τ;


void πshared_float(πφ &p)
{
    p .def_spost("+.", [](πse<f64> y, f64 x) { return x + y.x; })
      .def_spost("-.", [](πse<f64> y, f64 x) { return x - y.x; })
      .def_spost("*.", [](πse<f64> y, f64 x) { return x * y.x; })
      .def_spost("/.", [](πse<f64> y, f64 x) { return x / y.x; })
      .def_spost("%.", [](πse<f64> y, f64 x) { return fmod(x, y.x); })

      .def_spre("|.", [](f64 x) { return fabs(x); })
      .def_spre("_.", [](f64 x) { return -x; })

      .def_spre("S.", [](f64 x) { return sin(x); })
      .def_spre("C.", [](f64 x) { return cos(x); })
      .def_spre("T.", [](f64 x) { return tan(x); })
      .def_spre("L.", [](f64 x) { return log(x); })
      .def_spre("E.", [](f64 x) { return exp(x); })

      .def_spre("Θ.", [](πi &i, f64 x)
        { i.t()->Θ(now() + i64(x * 1e9) * 1ns);
          return x; })
      ;
}


}

#include "end.hh"
