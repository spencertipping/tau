#include "../tau.hh"
#include "begin.hh"

namespace τ
{


O &operator<<(O &s, ηmc &v)
{
  return s << v.y();
}


}

#include "end.hh"
