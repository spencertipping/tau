#include "serde.hh"
#include "topology.hh"
#include "../begin.hh"

namespace τ
{


γ γηbytes()
{
  return γffn("ηb", [](ξi i, ξo o)
    { for (let x : i)
      { η0o y(η0t::bytes);
        x.into(y.iptr(x.osize()));
        if (!(o << y)) return; }});
}


γ γbytesη()
{
  return γffn("bη", [](ξi i, ξo o)
    { B b;
      for (let x : i)
        if (x.tsb())
        { b.append(x.bv());
          uN j = 0;
          while (η1bc(b.data() + j, b.size() - j))
          { let y = η0i(b.data() + j);
            if (!(o << y)) return;
            j += y.osize(); }
          if (j) b = b.substr(j); }
        else if (x.τ()) b.clear(); });
}


}

#include "../end.hh"
