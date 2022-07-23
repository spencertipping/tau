#ifndef tau_fabric_monitor_h
#define tau_fabric_monitor_h


#include <cassert>
#include <chrono>
#include <cstdint>
#include <unordered_map>


#include "../coro.hh"


#include "../begin.hh"


namespace tau::fabric
{

using namespace std::literals;

namespace tc = tau::coro;


template<class F = uint64_t, class O = uint64_t, size_t N = 64>
struct log_histogram
{
  F n[N]{0};

  log_histogram &operator<<(O x) { ++n[ilog(x)]; return *this; }

  int icdf(double p) const
    { let t = total();
      if (!t) return 0;
      F c{0};
      F tc = static_cast<F>(t * p);
      for (int i = 0; i < N; ++i)
        if (c + n[i] > tc)
          return i;
      return N; }

  F total(int n_ = N) const
    { F t{0};
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

  bool     is_running{false};
  tp       last_start;
  span     total_elapsed{0};
  uint64_t n_splits;
  lh       splits;


  span mean_split() const
    { if (!n_splits) return 0ns;
      return total_elapsed / n_splits; }

  stopwatch &start()
    { assert(!is_running);
      last_start = std::chrono::steady_clock::now();
      is_running = true;
      return *this; }

  span stop()
    { assert(is_running);
      let n = std::chrono::steady_clock::now();
      let s = n - last_start;
      total_elapsed += s;
      splits << s.count();
      is_running = false;
      return s; }
};


template<class T>
struct cell_monitor
{
  typedef tc::coro<T, cell_monitor> co;

  stopwatch s;

  void init (co &c) {}
  void enter(co &c) { s.start(); }
  void exit (co &c) { s.stop();  }
  void finalize(co &c) {}
};


}

#include "../end.hh"


#endif
