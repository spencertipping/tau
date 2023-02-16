#include "pi.hh"
#include "share.hh"
#include "linux.hh"
#include "wasm.hh"

#include "begin.hh"

namespace τ
{


static φ<πfn> φγ_;

φd_<πfn> &φγd()
{ if (!φγ_) φγ_.reset(new φd_<πfn>);
  return *dpc<φd_<πfn>>(φγ_); }


φ<πfn> φγ()
{
  return φN("φγ", φm<V<πfn>, πfn>(φn(φws(φγ1()), 1), [](V<πfn> fs)
    { πfn r;
      for (let &x : fs)
      { r += x;
        r << πinsn{"γ|", [](πi &i)
          { let b = i.dpop().as_γ();
            let a = i.dpop().as_γ();
            i.dpush(a | b);
            return πinsn_ok; }}; }
      return r; }));
}


φ<πfn> φγ1()
{
  static bool i = false;
  if (!i)
  {
    φγd();
    i = true;

    φγd().def("[", φm<P<πfn, Op<int>>, πfn>(
      φs(φγ(), φo(φl("]", 0))),
      [](auto x) { return std::get<0>(x); }));

    φshare(φγd());
    φlinux(φγd());
    φwasm (φγd());
  }
  return φN("φγ₁", φγ_);
}


}

#include "end.hh"
