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
  ζb                *b;  // TODO: convert back to direct
  P<uf8c, ζi> const  i;
  S<λi>              rls;
  S<λi>              wls;
  bool               rc {false};
  bool               wc {false};

  ζ(Λ &l_, uf8 b_ = ζb0) : l(l_), b(new ζb(b_)), i(ζni(b_, this)) {}
  ~ζ() { ζs[std::get<0>(i)][std::get<1>(i)] = nullptr;
         delete b;}

  ζ &λw(S<λi> &ls) { for (let i : ls) l.r(i); ls.clear(); return *this; }

  ζ &rω() { rc = true; λw(wls); return *this; }
  ζ &wω() { wc = true; λw(rls); return *this; }

  bool operator<<(W const &x)
    { let s = x.size(b->c, b->wa());
      uN  a = -1;
      while ((a = b->alloc(s)) == Nl<uN>::max())
      { if (rc) return false;
        rls.insert(l.i());
        l.y(λO); }
      x.write(*b + a, s);
      λw(rls);
      return true; }

  u8 *operator+(uN a) const { return *b + a; }

  uN ref(uN a) const { return ζri(std::get<0>(i), std::get<1>(i), a); }


  // FIXME: ζ should not be its own iterator, even though it kinda makes
  // sense; we should delegate iteration to R
  //
  // (I believe this is causing double-free issues)
  R  operator*() const { return ref(b->ri); }
  ζ &operator++()
    { while (!b->ra())
      { if (wc) return *this;
        wls.insert(l.i()), l.y(λI); }
      R::free(*b + b->ri);
      b->free(b->ri + R::size_of(*b + b->ri));
      std::cout << "just freed: " << b << std::endl;
      λw(wls);
      return *this; }

  // NOTE: .begin(), .end(), and iterator may be the three destructor
  // calls -- although I'd expect for them to be aliased since they're
  // references
  ζ &begin() { return *this; }
  ζ &end()   { return *this; }

  // ζs are equal when both are closed; otherwise they are different
  // (this is only for iteration)
  bool operator==(ζ &x) const { return wc && !b->ra() && x.wc && !x.b->ra(); }
};


template<class R, class W = R> ζ<R, W> &rζ(uN r) { return *Rc<V<ζ<R, W>*>*>(ζs)[ζbi(r)][ζzi(r)]; }


#if tau_debug_iostream
template<class R, class W>
O &operator<<(O &s, ζ<R, W> const &x)
{
  return s << "ζ" << Sc<uN>(std::get<0>(x.i)) << ":" << std::get<1>(x.i)
           << " " << *x.b;
}
#endif


}

#include "end.hh"


#endif
