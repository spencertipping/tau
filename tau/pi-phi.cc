#include "types.hh"
#include "pi-phi.hh"
#include "pi-fn.hh"
#include "phi-ctor.hh"
#include "phi-fn.hh"
#include "phi-str.hh"

#include "begin.hh"

namespace τ
{


static φ<St> πws() { return φcs(" \t\n\r", false, 1); }
static φ<St> πlc() { return φq(φs(φl("# ", ""), φcs("\n", true))); }
static φ<St> πig() { return φq(φn(φa<St>(πws(), πlc()))); }

template<class X>
φ<X> πwrap(φ<X> p) { return φ2(φo(πig()), p, φo(πig())); }

template<class X>
φ<X> πgroup(φ<X> p) { return φ2(πwrap(φl("[", "")), πwrap(p), πwrap(φl("]", ""))); }


φ<πf> πφ(φ<πf> a, φ<πmf> m, φ<πdf> d, φ<πtf> t)
{
  let p1 = φa<πf>();  auto &p1a = p1.as<φa_<πf>>();
  let p  = φa<πf>();  auto &pa  = p .as<φa_<πf>>();
  let pr = φN("π", p);

  p1a << πgroup(pr) << πwrap(a);
  pa  << φm(φs(πwrap(m), pr),         [](auto r) { let &[m, x]       = r; return πmc(m, x); })
      << φm(φs(p1, πwrap(d), pr),     [](auto r) { let &[x, d, y]    = r; return πdc(d, x, y); })
      << φm(φs(p1, πwrap(t), pr, pr), [](auto r) { let &[x, t, y, z] = r; return πtc(t, x, y, z); });

  return p;
}


}

#include "end.hh"
