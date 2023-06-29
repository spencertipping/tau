#include <numeric>

#include "types.hh"
#include "pi-phi.hh"
#include "phi-ctor.hh"
#include "phi-fn.hh"
#include "phi-str.hh"

#include "begin.hh"

namespace τ
{


static π1 np(V<π1> &&ps)
{
  A(!ps.empty(), "np is not well-defined for empty lists");

  Ss ns;
  bool first = true;
  for (let &p : ps)
  {
    if (first) first = false;
    else       ns << "|";
    ns << p;
  }

  return π1{ns.str(), [ps=mo(ps)](πi &i)
    {
      uN s = 0;
      for (let &p : ps) p(i), s += i.peek().il;  // push each onto stack in reverse order
      let r = i.r(s + 8, [&](auto &&o)
        {
          for (iN j = ps.size() - 1; j >= 0; --j) o << i[i[j]].all();
          for (uN j = 0; j < ps.size(); ++j)      i.pop();
        });
      i.push(r);
    }};
}


φ<π1> πφnp(φ<π1> p)
{
  slet r = φn(φ1("p','?", p, φo(φco_())), 1) * np;
  return r;
}


}

#include "end.hh"
