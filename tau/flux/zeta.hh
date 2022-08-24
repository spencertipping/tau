#ifndef tau_flux_ζ_h
#define tau_flux_ζ_h


#include "../types.hh"

#include "Lambda.hh"
#include "zetab.hh"
#include "zetabits.hh"


#include "../module/begin.hh"

namespace tau::flux
{


typedef uf8 ζTi;


template<class T>
struct ζr;

template<class T>
struct ζ
{
  ζTi         const  rtti;
  Λ                 &l;
  P<uf8c, ζi> const  i;
  ζb                 b;
  S<λi>              rls;
  S<λi>              wls;
  bool               rc {false};
  bool               wc {false};

  ζ(Λ &l_, uf8 b_ = ζb0) : rtti(T::rtti()), l(l_), i(ζni(b_, this)), b(b_) {}
  ~ζ() { ζs[std::get<0>(i)][std::get<1>(i)] = nullptr; }

  ζ &λw(S<λi> &ls) const
    { for (let i : ls) l.r(i);
      ls.clear();
      return *this; }

  bool operator<<(T const &x)
    { assert(T::rtti() == rtti);
      let s = x.size(b.c, b.wa());
      iN  a = -1;
      while ((a = b.alloc(s)) == -1)
      { if (rc) return false;
        rls.insert(l.i());
        l.y(λO); }
      x.write(b + a, s);
      λw(rls);
      return true; }

  // FIXME: read-side operators should return ζr<T>
  bool operator++()
    { while (!b.ra())
      { if (wc) return false;
        wls.insert(l.i()), l.y(λI); }
      T::free(b + b.ri);
      b.free(b.ri + T::size_of(b + b.ri));
      λw(wls);
      return b.ra(); }

  T operator* ()     const { return (*this)[0]; }
  T operator[](uN a) const
    { assert(T::rtti() == rtti);
      assert(b[a]);
      let s = T::size_of(b + a);
      assert(b[a + s - 1]);
      return T(b + a, s); }
};


template<class T>
struct ζr
{
  uNc i;
  ζr(uN i_) : i(i_) {}

  uf8   bi() const { return ζbi(i); }
  ζi    zi() const { return ζzi(i); }
  uN    xi() const { return ζxi(i); }
  ζ<T> &z () const { return *Rc<ζ<T>*>(ζs[zi()]); }

  T operator*() const { return z()[xi()]; }
};


#if tau_debug_iostream
template<class T>
O &operator<<(O &s, ζr<T> const &x)
{
  return s << "ζr" << x.bi() << ":" << x.zi() << ":" << x.xi();
}


template<class T>
O &operator<<(O &s, ζ<T> const &x)
{
  return s << "ζ" << std::get<0>(x.i) << ":" << std::get<1>(x.i)
           << x.b;
}
#endif


}

#include "../module/end.hh"


#endif
