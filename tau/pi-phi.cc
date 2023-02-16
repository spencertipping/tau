#include "pi-phi.hh"
#include "begin.hh"

namespace τ
{


φ<πfn> φnull(πinsn i)
{
  return φl("", πfn{i});
}


φ<πfn> φinsn(φ<πfn> p, πinsn i)
{
  return φm<πfn, πfn>(p, [i](πfn f) { return f << i; });
}


}

#include "end.hh"
