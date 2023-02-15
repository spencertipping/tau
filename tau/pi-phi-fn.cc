#include "pi-phi.hh"
#include "begin.hh"

namespace τ
{


φ<πfn> φπ()
{
  return φm<P<πfn, bool>, πfn>(
    φs(φπ_literal(), φE()),
    [](P<πfn, bool> x) { return std::get<0>(x); });
}


}

#include "end.hh"
