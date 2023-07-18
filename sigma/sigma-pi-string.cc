#include <cppcodec/cppcodec/base64_rfc4648.hpp>

#include "sigma-Gamma-pieces.hh"
#include "begin.hh"

namespace σ
{

using namespace std::literals;
using namespace τ;


void πshared_string(πφ &p)
{
  using b64 = cppcodec::base64_rfc4648;
  p .def_spre("<64", [](Stvc &s) { let v = b64::decode(s); return St{v.begin(), v.end()}; })
    .def_spre(">64", [](Stvc &s) { let v = b64::encode(s); return St{v.begin(), v.end()}; })
    ;
}


}

#include "end.hh"
