#ifndef tau_fabric_stopwatch_h
#define tau_fabric_stopwatch_h


#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <numeric>

#include "../module/begin.hh"


namespace tau::fabric
{

using namespace std::literals;


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


struct stopwatch
{
  typedef std::chrono::steady_clock                              clk;
  typedef std::chrono::time_point<clk, std::chrono::nanoseconds> tp;
  typedef std::chrono::nanoseconds                               span;
  typedef log_histogram<uint64_t, uint64_t, 64>                  lh;

  bool     is_running    {false};
  tp       last_start    {0ns};
  span     total_elapsed {0};
  uint64_t n_splits      {0};
  lh       splits;


  span p(double p) const
    { return 1ns * (1ull << splits.icdf(p) + 1); }

  span elapsed() const
    { return total_elapsed + (is_running ? now() - last_start : 0ns); }

  stopwatch &operator<<(span s)
    { total_elapsed += s;
      splits << s.count();
      ++n_splits;
      return *this; }

  span mean_split() const
    { if (!n_splits) return 0ns;
      return total_elapsed / n_splits; }

  stopwatch &start()
    { assert(!is_running);
      last_start = now();
      is_running = true;
      return *this; }

  span stop()
    { assert(is_running);
      let s = now() - last_start;
      is_running = false;
      *this << s;
      return s; }

  static tp now()               { return clk::now(); }
  static constexpr tp never()   { return tp{0ns}; }
  static constexpr tp forever() { return tp{std::numeric_limits<tp::duration::rep>::max() * 1ns}; }
};


static std::ostream &operator<<(std::ostream &s, stopwatch::span t)
{
  if      (t <= 100us) return s << t.count() << "ns";
  else if (t <= 100ms) return s << t / 1us   << "μs";
  else if (t <= 100s)  return s << t / 1ms   << "ms";
  else if (t <= 1h)    return s << t / 1s    << "s";
  else if (t <= 10h)   return s << t / 1min  << "m";
  else                 return s << t / 1h    << "h";
  return s;
}


template<class F, class O, size_t N>
static std::ostream &operator<<(std::ostream &s, log_histogram<F, O, N> const &h)
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


static std::ostream &operator<<(std::ostream &s, stopwatch const &w)
{
  return s << "sw[" << (w.is_running ? "R" : "s")
           << " n=" << w.n_splits
           << " e=" << w.elapsed()
           << " p50=" << w.p(0.5)
           << " p90=" << w.p(0.9)
           << " p99=" << w.p(0.99) << "]";
}


}

#include "../module/end.hh"


#endif
