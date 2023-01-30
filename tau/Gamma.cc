#include "Gamma.hh"
#include "begin.hh"

namespace τ
{


O &operator<<(O &s, Γ const &g)
{
  return s << g.name();
}


}

#include "end.hh"
