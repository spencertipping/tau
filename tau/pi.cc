#include "pi.hh"
#include "share.hh"
#include "linux.hh"
#include "wasm.hh"

#include "begin.hh"

namespace τ
{


φ<γ> φγ()
{
  let r = new φd_<γ>;
  φshare(*r);
  φlinux(*r);
  φwasm(*r);
  return Sp<φ_<γ>>(r);
}


}

#include "end.hh"
