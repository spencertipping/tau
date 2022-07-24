#ifndef tau_fabric_stopwatch_h
#define tau_fabric_stopwatch_h


#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>

#include "../begin.hh"


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
      if (x >> 32) i += 32, x &= 0xffffffffull;
      if (x >> 16) i += 16, x &= 0xffffull;
      if (x >> 8)  i += 8,  x &= 0xffull;
      if (x >> 4)  i += 4,  x &= 0xfull;
      if (x >> 2)  i += 2,  x &= 0x3ull;
      if (x >> 1)  i++;
      return i; }
};


struct stopwatch
{
  typedef std::chrono::steady_clock             clk;
  typedef std::chrono::time_point<clk>          tp;
  typedef std::chrono::nanoseconds              span;
  typedef log_histogram<uint64_t, uint64_t, 64> lh;

  bool     is_running    {false};
  tp       last_start    {0ns};
  span     total_elapsed {0};
  uint64_t n_splits      {0};
  lh       splits;


  span elapsed() const
    { assert(is_running);
      return now() - last_start; }

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

  static tp now() { return std::chrono::steady_clock::now(); }
};


static std::ostream &operator<<(std::ostream &s, stopwatch::span t)
{
  if      (t <= 100us) return s << t.count() << "ns";
  else if (t <= 100ms) return s << t / 1us << "μs";
  else if (t <= 100s)  return s << t / 1ms << "ms";
  else if (t <= 10h)   return s << t / 1s  << "s";
  else                 return s << t / 1h  << "h";
  return s;
}


}

#include "../end.hh"


#endif
