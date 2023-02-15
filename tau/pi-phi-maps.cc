#include "pi-phi.hh"
#include "begin.hh"

namespace τ
{


static πinsn φmap_push()
{
  return πinsn{"mpush", [](πi &i)
    { i.wv() << i.dpop().as_η(); return πinsn_ok; }};
}


φ<πfn> φmap_element()
{
  static φ<πfn> r;
  if (!r)
    r = φm<P<πfn, πfn>, πfn>(
      φs(φk(φcs("abcdefghijklmnopqrstuvwxyz", false, 1)), φπ()), [](auto p)
        { πfn f;
          f += std::get<0>(p); f << φmap_push();
          f += std::get<1>(p); f << φmap_push();
          return f; });
  return r;
}


φ<πfn> φmap()
{
  static φ<πfn> r;
  if (!r)
    r = φm<P<V<πfn>, Op<int>>, πfn>(
      φs(φn(φmap_element()), φo(φl("}", 0))),
      [](P<V<πfn>, Op<int>> p)
        { πfn r;
          r << πinsn{"mbegin", [](πi &i) { i.wbegin(η0t::map); return πinsn_ok; }};
          for (let &x : std::get<0>(p)) r += x;
          r << πinsn{"mend", [](πi &i) { i.wend(); return πinsn_ok; }};
          return r; });
  return r;
}


}

#include "end.hh"
