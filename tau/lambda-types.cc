#include "../tau.hh"
#include "begin.hh"

namespace τ
{


O &operator<<(O &s, λs t)
{
  switch (t)
  {
  case λs::R:  return s << "R";
  case λs::S:  return s << "S";
  case λs::X:  return s << "X";
  case λs::Y:  return s << "Y";
  case λs::Θ:  return s << "Θ";
  case λs::ξR: return s << "ξR";
  case λs::ξW: return s << "ξW";
  case λs::τR: return s << "τR";
  case λs::τW: return s << "τW";
  case λs::τE: return s << "τE";
  case λs::τA: return s << "τA";
  default:     return s << "λs " << Sc<int>(t);
  }
}


}

#include "end.hh"
