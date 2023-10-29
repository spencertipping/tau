#include "pre-sstream.hh"
#include "../tau/begin.hh"

namespace σ::pre
{


ηsstream ηisstream(ηic &x)
{
  return ηsstream(new ηisstream_<ηsstream_fast>(x));
}

ηsstream ηsstream_union(Vc<ηsstream> &xs)
{
  return ηss_bin_apply(V<ηsstream>{xs}, [](ηsstream &x, ηsstream &y) { return x | y; });
}

ηsstream ηsstream_intersect(Vc<ηsstream> &xs)
{
  return ηss_bin_apply(V<ηsstream>{xs}, [](ηsstream &x, ηsstream &y) { return x & y; });
}


}

#include "../tau/end.hh"
