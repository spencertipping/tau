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


// TODO: break this into a "write side" and a "read side", both
// some type of φ rather than ζ
struct ζ
{
  Λ     &l;
  ζb     b;
  S<λi>  rls;
  S<λi>  wls;
  bool   rc {false};
  bool   wc {false};

  ζ(Λ &l_, uf8 b_ = ζb0) : l(l_), b(b_) {}
  ζ(ζ &x) = delete;

  ζ &λw(S<λi> &ls)
    { if (!ls.empty()) { for (let i : ls) l.r(i); ls.clear(); }
      return *this; }

  ζ &rω() { rc = true; λw(wls); return *this; }
  ζ &wω() { wc = true; λw(rls); return *this; }

  template<class W>
  bool operator<<(W const &x)
    { let s = x.size(b.c, b.wa());
      uN  a = -1;
      while ((a = b.alloc(s)) == Nl<uN>::max())
      { if (rc) return false;
        wls.insert(l.i());
        l.y(λO); }
      x.write(b + a, s);
      λw(rls);
      return true; }

  u8 *operator+(uN a) const { return b + a; }

  template<class R>
  struct it
  {
    ζ &z;

    it operator++()
      { while (!z.b.ra())
        { if (z.wc) return *this;
          z.rls.insert(z.l.i()), z.l.y(λI); }
        R::free(z.b + z.b.ri);
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
