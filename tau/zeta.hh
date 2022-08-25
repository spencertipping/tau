#ifndef tau_flux_ζ_h
#define tau_flux_ζ_h


#include "arch.hh"
#include "types.hh"

#include "Lambda.hh"
#include "gate.hh"
#include "zetab.hh"


#include "begin.hh"

namespace tau
{


#if tau_wordsize == 64
  uf8c constexpr ζb0 = 16;
#elif tau_wordsize == 32
  ut8c constexpr ζb0 = 12;
#else
# error unsupported word size for ζ
#endif


struct ζ
{
  ζb     b;
  λg     rg;
  λg     wg;
  bool   rc {false};
  bool   wc {false};

  ζ(ζ &x) = delete;
  ζ(Λ &l, uf8 b_ = ζb0) : b(b_), rg{l}, wg{l} {}


  ζ &rω() { rc = true; wg.w(); return *this; }
  ζ &wω() { wc = true; rg.w(); return *this; }


  u8 *operator+(uN a) const { return b + a; }
  uN          a()     const { return b.ri; }
  bool       ra()     const { return b.ra(); }
  bool       wa()     const { return b.wa(); }
  bool       ri()     const { return !wc || ra(); }
  bool       wi()     const { return !rc; }


  template<class R>
  u8 *r()
    { while (!b.ra()) { if (wc) return Rc<u8*>(-1); rg.y(λI); }
      let a = b + b.ri;
      b.free(b.ri + R::size_of(a));
      wg.w();
      return a; }


  template<class W>
  bool w(W const &x)
    { let s = x.size(b.c, b.wa());
      uN  a = -1;
      while ((a = b.alloc(s)) == Nl<uN>::max())
      { if (rc) return false; wg.y(λO); }
      x.write(b + a, s);
      rg.w();
      return true; }
};


#if tau_debug_iostream
O &operator<<(O &s, ζ const &x)
{
  return s << "ζ" << (x.rc ? "#" : "r") << (x.wc ? "#" : "w") << " " << x.b;
}
#endif


}

#include "end.hh"


#endif
