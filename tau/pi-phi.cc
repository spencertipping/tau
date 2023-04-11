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
  let a_ = πwrap(a);
  let m_ = πwrap(m);
  let d_ = πwrap(d);
  let t_ = πwrap(t);
  let g_ = πgroup(a);

  TODO("finish πφ()");
}


}

#include "end.hh"
