#include "gamma.hh"
#include "begin.hh"

namespace τ
{


O &operator<<(O &s, γ const &g)
{
  return s << g.name();
}


}

#include "end.hh"
