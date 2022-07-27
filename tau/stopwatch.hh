#ifndef tau_kern_stopwatch_h
#define tau_kern_stopwatch_h


#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <numeric>


#include "kern/log-histogram.hh"


#include "module/begin.hh"

namespace tau
{

using namespace std::literals;


struct stopwatch
{
  typedef std::chrono::steady_clock                              clk;
  typedef std::chrono::time_point<clk, std::chrono::nanoseconds> tp;
  typedef std::chrono::nanoseconds                               span;
  typedef kern::log_histogram<uint64_t, uint64_t, 64>            lh;

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


std::ostream &operator<<(std::ostream &s, stopwatch::span const &t)
{
  if      (t <= 100us) return s << t.count() << "ns";
  else if (t <= 100ms) return s << t / 1us   << "μs";
  else if (t <= 100s)  return s << t / 1ms   << "ms";
  else if (t <= 1h)    return s << t / 1s    << "s";
  else if (t <= 10h)   return s << t / 1min  << "m";
  else                 return s << t / 1h    << "h";
  return s;
}


std::ostream &operator<<(std::ostream &s, stopwatch const &w)
{
  return s << "sw[" << (w.is_running ? "R" : "s")
           << " n=" << w.n_splits
           << " e=" << w.elapsed()
           << " p50=" << w.p(0.5)
           << " p90=" << w.p(0.9)
           << " p99=" << w.p(0.99) << "]";
}


}

#include "module/end.hh"


#endif
