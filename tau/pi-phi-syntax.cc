#include "pi-phi.hh"
#include "begin.hh"

namespace τ
{


slet φspace   = φm<St, πfn>(φcs(" \t\n"), [](St) { return πfn(); });
slet φcomment = φm<P<int, St>, πfn>(φs(φl("# ", 0), φcs("\n", true)),
                                    [](auto) { return πfn(); });


slet φws_ = φa<πfn>(φspace, φcomment);


φ<πfn> φws()
{
  return φws_;
}


}

#include "end.hh"
