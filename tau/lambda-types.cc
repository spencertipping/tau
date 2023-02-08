#include "lambda-types.hh"
#include "begin.hh"

namespace τ
{


O &operator<<(O &s, λs t)
{
  switch (t)
  {
  case λs::R:  return s << "R";
  case λs::S:  return s << "S";
  case λs::Y:  return s << "Y";
  case λs::Θ:  return s << "Θ";
  case λs::ξR: return s << "ξR";
  case λs::ξW: return s << "ξW";
  case λs::τR: return s << "τR";
  case λs::τW: return s << "τW";
  case λs::τE: return s << "τE";
    TA(s, "illegal λs " << Sc<uN>(t));
  }
}


}

#include "end.hh"
