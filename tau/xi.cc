#include "xi.hh"
#include "begin.hh"

namespace τ
{


O &operator<<(O &s, ξ const &y)
{
  return s << "ξ[" << (y.wc ? "#" : "")
           << "wt=" << y.wt() << " " << y.z << "]";
}



}

#include "end.hh"
