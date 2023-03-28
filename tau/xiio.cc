#include "xiio.hh"
#include "begin.hh"

namespace τ
{


O &operator<<(O &s, ξd const &x)
{
  return s << x.f.inner_ξ().get() << "/" << x.b.inner_ξ().get();
}


}

#include "end.hh"
