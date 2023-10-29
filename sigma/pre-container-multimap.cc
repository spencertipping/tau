#include "pre-container-multimap.hh"
#include "begin.hh"

namespace σ::pre
{


void kvmmat_::α(ηic &k, ηic &v, ξo)
{
  stage(k, v);
}


void kvmmat_::ι(ηic &k, ηic&, ξo o)
{
  auto i = at(k);
  while (*i) o.r() << **i, ++*i;
  o.r() << ηsig::τ;
}


}

#include "end.hh"
