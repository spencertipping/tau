#ifndef tau_flux_ζ_h
#define tau_flux_ζ_h


#include "arch.hh"
#include "types.hh"

#include "Lambda.hh"
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
  Λ     &l;
  ζb     b;
  V<λi>  rls;
  V<λi>  wls;
  bool   rc {false};
  bool   wc {false};

  ζ(ζ &x) = delete;
  ζ(Λ &l_, uf8 b_ = ζb0) : l(l_), b(b_)
    { rls.reserve(4);
      wls.reserve(4); }


  ζ &λw(V<λi> &ls)
    { for (let i : ls) l.r(i); if (!ls.empty()) ls.erase(ls.begin(), ls.end());
      return *this; }

  ζ &rω() { rc = true; λw(wls); return *this; }
  ζ &wω() { wc = true; λw(rls); return *this; }


  u8 *operator+(uN a) const { return b + a; }
  uN          a()     const { return b.ri; }
  bool       ra()     const { return b.ra(); }
  bool       wa()     const { return b.wa(); }


  template<class W>
  bool operator<<(W const &x)
    { let s = x.size(b.c, b.wa());
      uN  a = -1;
      while ((a = b.alloc(s)) == Nl<uN>::max())
      { if (rc) return false;
        wls.push_back(l.i());
        l.y(λO); }
      x.write(b + a, s);
      λw(rls);
      return true; }


  // TODO: convert this to a pair of methods, operator++ and operator*,
  // to be used by φ

  template<class R>
  struct it
  {
    ζ &z;

    it operator++()
      { while (!z.b.ra())
        { if (z.wc) return *this;
          z.rls.push_back(z.l.i()), z.l.y(λI); }
        R::free(z + z.a());
        z.b.free(z.b.ri + R::size_of(z.b + z.b.ri));
        z.λw(z.wls);
        return *this; }

    R    operator*()      const { return z.b + z.b.ri; }
    bool operator==(it x) const { return z.wc && !z.b.ra(); }
  };

  template<class R> it<R> begin() { return it<R>{*this}; }
  template<class R> it<R> end()   { return it<R>{*this}; }
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
