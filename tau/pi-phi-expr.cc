#include "pi.hh"
#include "share.hh"
#include "linux.hh"
#include "wasm.hh"

#include "begin.hh"

namespace τ
{


static φ<πfn> φγ_;
static φ<πfn> φπ_;
static φ<πfn> φπd_;

φd_<πfn> &φγd() { if (!φγ_)  φγ_ .reset(new φd_<πfn>); return *dpc<φd_<πfn>>(φγ_); }
φa_<πfn> &φπa() { if (!φπ_)  φπ_ .reset(new φa_<πfn>); return *dpc<φa_<πfn>>(φπ_); }
φd_<πfn> &φπd() { if (!φπd_) φπd_.reset(new φd_<πfn>); return *dpc<φd_<πfn>>(φπd_); }


φ<πfn> φγ()
{
  static bool i = false;
  if (!i)
  {
    φγd();
    i = true;
    φshare(φγd());
    φlinux(φγd());
    φwasm (φγd());
  }
  return φN("φγ", φws(φγ_));
}


φ<πfn> φπ()
{
  static bool i = false;
  if (!i)
  {
    φπa();
    φπd();
    i = true;
    φπa() << φπ_literal();
    φπa() << φπd_;
    φπd().def("(", φtuple());
  }
  return φN("φπ", φws(φπ_));
}


}

#include "end.hh"
