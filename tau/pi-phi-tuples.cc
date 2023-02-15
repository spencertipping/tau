#include "pi-phi.hh"
#include "begin.hh"

namespace τ
{


φ<πfn> φtuple_element()
{
  static φ<πfn> r;
  if (!r)
    r = φm<πfn, πfn>(φπ(), [](πfn f)
      { return f << πinsn{"tpush", [](πi &i)
        { i.wv() << i.dpop().as_η(); return πinsn_ok; }}; });
  return r;
}


φ<πfn> φtuple()
{
  static φ<πfn> r;
  if (!r)
    r = φm<P<V<πfn>, Op<int>>, πfn>(
      φs(φn(φtuple_element()), φo(φl(")", 0))),
      [](P<V<πfn>, Op<int>> p)
        { πfn r;
          r << πinsn{"tbegin", [](πi &i) { i.wbegin(η0t::tuple); return πinsn_ok; }};
          for (let &x : std::get<0>(p)) r += x;
          r << πinsn{"tend", [](πi &i) { i.wend(); return πinsn_ok; }};
          return r; });
  return r;
}


}

#include "end.hh"
