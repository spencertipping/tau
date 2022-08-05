#ifndef tau_util_log_histogram_h
#define tau_util_log_histogram_h


#include <cstddef>
#include <iostream>


#include "../types.hh"
#include "../util/numerics.hh"


#include "../module/begin.hh"

namespace tau::util
{


template<class F = u64, class O = u64, unsigned N = 64>
struct log_histogram
{
  F n[N]{0};

  log_histogram &operator<<(O x) { ++n[numerics::ilog(x)]; return *this; }

  int icdf(double p) const
    { let t = total();
      if (!t) return 0;
      F c  = 0;
      F tc = static_cast<F>(t * p);
      for (unsigned i = 0; i < N; ++i)
        if ((c += n[i]) > tc)
          return i;
      return N; }

  F total(unsigned n_ = N) const
    { F t = 0;
      for (unsigned i = 0; i < n_; ++i) t += n[i];
      return t; }


  log_histogram<F, O, N> &operator+=(log_histogram<F, O, N> const &x)
    { for (unsigned i = 0; i < N; ++i) n[i] += x.n[i];
      return *this; }
};


template<class F, class O, unsigned N>
std::ostream &operator<<(std::ostream &s, log_histogram<F, O, N> const &h)
{
  F        m  = h.n[0]; for (int i = 1; i < N; ++i) m = std::max(m, h.n[i]);
  unsigned u  = N - 1;  while (u > 0 && !h.n[u]) --u;
  let      ml = numerics::ilog(m);

  u = std::min(N, u + 7 & ~7);
  if (ml)
    for (int i = 0; i < u; ++i)
      s << ".123456789abcdef|"[numerics::ilog(h.n[i]) * 16 / ml]
        << (i + 1 & 7 ? "" : " ");

  return s;
}


}

#include "../module/end.hh"


#endif
