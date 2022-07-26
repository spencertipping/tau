#ifndef tau_fabric_pipe_h
#define tau_fabric_pipe_h


#include <cassert>
#include <cmath>
#include <iostream>
#include <queue>
#include <stdexcept>
#include <string>
#include <utility>


#include "../coro.hh"

#include "stopwatch.hh"


#include "../begin.hh"


namespace tau::fabric
{


template<class T>
struct pipe
{
  typedef std::pair<stopwatch::tp, T> qe;

  stopwatch      read_delay;
  stopwatch      write_delay;
  stopwatch      latency;
  size_t         capacity;
  std::deque<qe> xs;

  pipe() {}
  pipe(size_t c_) : capacity(c_) { assert(capacity); }


  pipe<T> &operator=(pipe<T> &&p)
    { read_delay  = p.read_delay;
      write_delay = p.write_delay;
      latency     = p.latency;
      capacity    = p.capacity;
      xs          = std::move(p.xs);
      return *this; }


  bool readable() const { return xs.size() > 0; }
  bool writable() const { return xs.size() < capacity; }

  void   close()        { capacity = 0; }
  bool   closed() const { return !capacity; }
  size_t size()   const { return xs.size(); }

  size_t total_read()    const { return latency.n_splits; }
  size_t total_written() const { return total_read() + xs.size(); }


  stopwatch::span λ() const
    { if (latency.n_splits + xs.size() == 0) return 0ns;
      auto t = 0ns;
      let  n = stopwatch::now();
      for (let &x : xs) t += n - std::get<0>(x);
      return (t + latency.total_elapsed) / (latency.n_splits + xs.size()); }

  double σ() const
    { return log2(static_cast<double>(write_delay.mean_split().count()))
           - log2(static_cast<double>(read_delay .mean_split().count())); }


  bool write(T const &x)
    { let n = stopwatch::now();
      if (!writable()) return false;
      xs.push_back(qe(n, x));
      return true; }


  bool has_next() { return readable(); }
  T next()
    { assert(readable());
      let x = std::get<1>(xs.front());
      latency << stopwatch::now() - std::get<0>(xs.front());
      xs.pop_front();
      return x; }
};


template<class T>
static std::ostream &operator<<(std::ostream &s, pipe<T> const &p)
{
  return s << "pipe["
           << (p.readable() ? "R" : "r")
           << (p.writable() ? "W" : p.closed() ? "#" : "w")
           << " n="  << p.total_written()
           << " c="  << p.size() << "/" << p.capacity
           << " rd=" << p.read_delay.mean_split()
           << " wd=" << p.write_delay.mean_split()
           << " λ="  << p.λ()
           << " σ="  << p.σ() << "]";
}


}


#include "../end.hh"

#endif
