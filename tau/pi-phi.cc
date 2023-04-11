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
  let p1 = φa<πf>();
  let pw = πwrap(p1);
  let po = φa<F<πf(πf)>>(
    φm(φs(πwrap(d), pw), [](auto x) -> F<πf(πf)>
      { let &[d, y] = x;
        return [d=d, y=y](πf x) { return πdc(d, x, y); }; }),
    φm(φs(πwrap(t), pw, pw), [](auto x) -> F<πf(πf)>
      { let &[t, y, z] = x;
        return [t=t, y=y, z=z](πf x) { return πtc(t, x, y, z); }; }));

  φ<πf> mo = φm(φs(πwrap(m), pw), [](auto r)
    { let &[m, x] = r; return πmc(m, x); });

  φ<πf> p = φN("π", φm(φs(p1, φn(po)), [](auto r)
    { let &[x, os] = r;

      // OOPS: we need to keep the intermediate values around so we can
      // apply this function correctly; right now we're left-associative
      //
      // dyadic and triadic operators can't capture their operands, which
      // might actually cause parsing problems...?
      return x; }));

  // Now add the alternatives
  p1.as<φa_<πf>>().push(πwrap(a), mo, πgroup(p));

  return p;
}


}

#include "end.hh"
