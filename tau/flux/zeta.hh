#ifndef tau_flux_zeta_h
#define tau_flux_zeta_h


#include "../types.hh"

#include "Lambda.hh"
#include "zetab.hh"


#include "../module/begin.hh"

namespace tau::flux
{


#if tau_wordsize == 32
# define tau_ζr_small 1
#elif tau_wordsize == 64
# define tau_ζr_small 0
#else
# error unsupported word size for ζ
#endif


#if tau_ζr_small
  u8c constexpr ζb0 = 12;  // default capacity (bits)
  u8c constexpr ζba = 2;   // bits allocated to bin selection

  // number of ζ bits for each bin
  // |ζ| = 1 << 30 - ζzb[i]
  u8c constexpr ζzb[4] = { 6, 10, 14, 18 };
#else
  u8c constexpr ζb0 = 16;
  u8c constexpr ζba = 3;

  // in 64-bit address space, we'll need at most 48 bits of ζ addressing
  // |ζ| = 1 << 61 - ζzb[i]
  u8c constexpr ζzb[8] = { 13, 17, 21, 25, 29, 33, 37, 41 };
#endif


// ζ address bits for a given ζba-sized prefix
inline u8 ζab(u8 a) { return tau_wordsize - ζba - ζzb[a]; }


V<void*> ζs[1 << ζba];


P<u8, ζi> ζni(u8 b, void *x)
{
  // Start with the |ζ|-smallest bin that can contain the ζ,
  // then work upwards
  for (u8 i = 0; i < ζba; ++i)
  {
    if (b > ζzb[i]) continue;
    for (ζi j = 0; j < ζs[i].size(); ++j)
      if (!ζs[i][j])
      {
        ζs[i][j] = x;
        return std::make_pair(i, j);
      }

    if (ζs[i].size() < 1 << ζzb[i])
    {
      ζs[i].push_back(x);
      return std::make_pair(i, ζs[i].size() - 1);
    }
  }

  // No more addresses and/or ζ entries left; this is very bad
  return std::make_pair(255, -1);
}


template<class T>
struct ζr;

template<class T, class RC = u8>
struct ζ
{
  Λ                &l;
  P<u8c, ζi> const  i;
  ζb                b;

  ζ(Λ &l_, u8 b_ = ζb0) : l(l_), i(ζni(b_, this)), b(b_) {}
  ~ζ() { ζs[std::get<0>(i)][std::get<1>(i)] = nullptr; }

  RC   rc(uN a) const { return R<RC>(b + a, 0); }
  void rc(uN a, RC x) {        W<RC>(b + a, 0, x); }

  ζ &claim  (uN a) { rc(a, rc(a) + 1); return *this; }
  ζ &unclaim(uN a) { rc(a, rc(a) - 1); return *this; }
};


template<class T>
struct ζr
{
  uNc i;
  ζr(uN i_) : i(i_) { z().claim  (xi()); }
  ~ζr()             { z().unclaim(xi()); }

  u8 bi() const { return i >> tau_wordsize - ζba; }
  ζi zi() const { return i >> ζab(bi()) & (1 << ζzb[bi()]) - 1; }
  uN xi() const { return i              & (1 << ζab(bi())) - 1; }

  ζ<T> &z() const { return *Rc<ζ<T>*>(ζs[zi()]); }
};


}

#include "../module/end.hh"


#endif
