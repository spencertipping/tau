#include <iostream>

#include "io.hh"
#include "topology.hh"
#include "../strings.hh"

#include "../begin.hh"

namespace τ
{


Sp<γ> γostream(O &s, St prefix, bool tap)
{
  return γffn("γ»", [=, &s](Sp<ψ>, ξi i, ξo o) mutable
  { for (let x : i)
    { s << prefix << x << std::endl;
      if (tap && !(o << x)) break; }});
}


Sp<γ> γtsv()
{
  return γffn("γtsv", [](Sp<ψ>, ξi i, ξo o) mutable
    { V<uN> ts; ts.reserve(64);
      for (let x : i)
        if (x.tsu() || x.tsb())
        { ts.clear();
          cs7{"\t"}.find(x.stv(), ts);
          η0o r(η0t::tuple);
          for (uN j = 0; j < ts.size() - 1; ++j)
            r << η1o(x.stv().substr(ts[j] + 1, ts[j + 1] - ts[j]));
          r << η1o(x.stv().substr(ts.empty() ? 0 : ts.back() + 1));
          if (!(o << r)) break; }});
}



}

#include "../end.hh"
