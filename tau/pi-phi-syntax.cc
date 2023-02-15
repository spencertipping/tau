#include "pi-phi.hh"
#include "begin.hh"

namespace τ
{


static φ<πfn> φspace()
{ return φm<St, πfn>(φcs(" \t\n"), [](St) { return πfn(); }); }

static φ<πfn> φcomment()
{ return φm<P<int, St>, πfn>(φs(φl("# ", 0), φcs("\n", true)),
                             [](auto) { return πfn(); }); }


φ<πfn> φws()
{
  static φ<πfn> r;
  if (!r) r = φa<πfn>(φspace(), φcomment());
  return r;
}


φ<πfn> φws(φ<πfn> p)
{
  return φm<P<πfn, P<πfn, πfn>>, πfn>(
    φs(φws(), φs(p, φws())),
    [](P<πfn, P<πfn, πfn>> x) { return std::get<0>(std::get<1>(x)); });
}


}

#include "end.hh"
