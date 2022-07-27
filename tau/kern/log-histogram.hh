#ifndef tau_kern_log_histogram_h
#define tau_kern_log_histogram_h


#include <cstdint>
#include <cstddef>
#include <iostream>


#include "../module/begin.hh"

namespace tau::kern
{


template<class F = uint64_t, class O = uint64_t, size_t N = 64>
struct log_histogram
{
  F n[N]{0};

  log_histogram &operator<<(O x) { ++n[ilog(x)]; return *this; }

  int icdf(double p) const
    { let t = total();
      if (!t) return 0;
      F c  = 0;
      F tc = static_cast<F>(t * p);
      for (int i = 0; i < N; ++i)
        if ((c += n[i]) > tc)
          return i;
      return N; }

  F total(int n_ = N) const
    { F t = 0;
      for (int i = 0; i < n_; ++i) t += n[i];
      return t; }


  static inline int ilog(uint64_t x)
    { int i = 0;
      if (x >> 32) i += 32, x >>= 32;
      if (x >> 16) i += 16, x >>= 16;
      if (x >> 8)  i += 8,  x >>= 8;
      if (x >> 4)  i += 4,  x >>= 2;
      if (x >> 2)  i += 2,  x >>= 1;
      if (x >> 1)  i++;
      return i; }
};


template<class F, class O, size_t N>
std::ostream &operator<<(std::ostream &s, log_histogram<F, O, N> const &h)
{
  F      m  = h.n[0]; for (int i = 1; i < N; ++i) m = std::max(m, h.n[i]);
  size_t u  = N - 1;  while (u > 0 && !h.n[u]) --u;
  let    ml = log_histogram<F, O, N>::ilog(m);

  u = std::min(N, u + 7 & ~7);
  if (ml)
    for (int i = 0; i < u; ++i)
      s << ".123456789abcdef|"[log_histogram<F, O, N>::ilog(h.n[i]) * 16 / ml]
        << (i + 1 & 7 ? "" : " ");

  return s;
}


}

#include "../module/end.hh"


#endif
