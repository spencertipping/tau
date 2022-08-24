#ifndef tau_flux_ζbits_h
#define tau_flux_ζbits_h


#include "../arch.hh"
#include "../types.hh"


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
inline uf8 ζab(uf8 a) { return tau_wordsize - ζba - ζzb[a]; }
inline uf8 ζbi(uN  a) { return a >> tau_wordsize - ζba; }
inline ζi  ζzi(uN  a) { return a >> ζab(ζbi(a)) & (1 << ζzb[ζbi(a)]) - 1; }
inline uN  ζxi(uN  a) { return a                & (1 << ζab(ζbi(a))) - 1; }


// Global lookup tables for Ζ<T> buffers, by size group
V<void*> ζs[1 << ζba];


P<u8, ζi> ζni(uf8 b, void *x)
{
  // Start with the |ζ|-smallest bin that can contain the ζ,
  // then work upwards looking for available entries
  for (uf8 i = 0; i < ζba; ++i)
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


}

#include "../module/end.hh"


#endif
