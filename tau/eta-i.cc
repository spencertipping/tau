#include "eta-i.hh"
#include "begin.hh"

namespace τ
{


O &operator<<(O &s, η const &y)
{
  return s << η1i(y.y1());
}


}

#include "end.hh"
