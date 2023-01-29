#include "tau-common.hh"
#include "begin.hh"

namespace τ
{


O &operator<<(O &s, τΘ const &h)
{
  return s << "τΘ:" << h.h << ":" << h.l;
}


}

#include "end.hh"
