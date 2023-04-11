#include "pi-fn-eta.hh"
#include "begin.hh"

namespace τ
{


πtf πηcond()
{
  return [](πi &i, πfc &x, πfc &y, πfc &z) -> πv
    { return x(i).truthy() ? y(i) : z(i); };
}


}

#include "end.hh"
