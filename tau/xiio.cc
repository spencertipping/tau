#include "../tau.hh"
#include "begin.hh"

namespace τ
{


O &operator<<(O &s, ξd const &x)
{
  return s << "ξd:f" << x.f.inner_ξ().get() << ":b" << x.b.inner_ξ().get();
}


}

#include "end.hh"
