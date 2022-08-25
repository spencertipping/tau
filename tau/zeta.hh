#ifndef tau_flux_ζ_h
#define tau_flux_ζ_h


#include "types.hh"

#include "Lambda.hh"
#include "zetab.hh"
#include "zetabits.hh"


#include "begin.hh"

namespace tau
{


template<class R, class W = R>
struct ζ
{
  Λ                 &l;
  ζb                 b;
  P<uf8c, ζi> const  i;
  S<λi>              rls;
  S<λi>              wls;
  bool               rc {false};
  bool               wc {false};

  ζ(Λ &l_, uf8 b_ = ζb0) : l(l_), b(b_), i(ζni(b_, this)) {}
  ζ(ζ &x) = delete;

  ~ζ() { ζs[std::get<0>(i)][std::get<1>(i)] = nullptr; }


  ζ &λw(S<λi> &ls)
    { if (!ls.empty()) { for (let i : ls) l.r(i); ls.clear(); }
      return *this; }

  ζ &rω() { rc = true; λw(wls); return *this; }
  ζ &wω() { wc = true; λw(rls); return *this; }

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

  uN ref(uN a) const { return ζri(std::get<0>(i), std::get<1>(i), a); }

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

    //R    operator* ()     const { return z.ref(z.b.ri); }
    R    operator*()      const { return z.b + z.b.ri; }
    bool operator==(it x) const { return z.wc && !z.b.ra(); }
  };

  it begin() { return it{*this}; }
  it end()   { return it{*this}; }
};


template<class R, class W = R> ζ<R, W> &rζ(uN r) { return *Rc<V<ζ<R, W>*>*>(ζs)[ζbi(r)][ζzi(r)]; }


#if tau_debug_iostream
template<class R, class W>
O &operator<<(O &s, ζ<R, W> const &x)
{
  return s << "ζ" << Sc<uN>(std::get<0>(x.i)) << ":" << std::get<1>(x.i)
           << " " << x.b;
}
#endif


}

#include "end.hh"


#endif
