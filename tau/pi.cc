#include "pi.hh"
#include "share.hh"
#include "linux.hh"
#include "wasm.hh"

#include "begin.hh"

namespace τ
{


φ<πγf> φγ()
{
  let r = new φd_<πγf>;
  φshare(*r);
  φlinux(*r);
  φwasm(*r);
  return Sp<φ_<πγf>>(r);
}


}

#include "end.hh"
