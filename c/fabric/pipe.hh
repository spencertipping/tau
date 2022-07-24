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

namespace tc = tau::coro;


struct null_interceptor
{
  void intercept() {}
};

null_interceptor the_null_interceptor{};


template<class T, class I = null_interceptor>
struct pipe
{
  typedef std::pair<stopwatch::tp, T> qe;

  stopwatch       read_delay;
  stopwatch       write_delay;
  stopwatch       latency;
  size_t          capacity;
  std::queue<qe>  xs;
  I              &interceptor;

  pipe(size_t capacity_)
    : capacity(capacity_),
      interceptor(the_null_interceptor)
    { assert(capacity); }

  pipe(size_t capacity_, I &interceptor_)
    : capacity(capacity_),
      interceptor(interceptor_)
    { assert(capacity); }


  bool readable() const { return xs.size() > 0; }
  bool writable() const { return xs.size() < capacity; }

  void await_read()  const { while (!closed() && !readable()) tc::yield(); }
  void await_write() const { while (!closed() && !writable()) tc::yield(); }

  operator bool() const { await_read(); return readable(); }

  void   close()        { capacity = 0; }
  bool   closed() const { return !capacity; }
  size_t size()   const { return xs.size(); }

  double pressure() const
    { return log2(static_cast<double>(write_delay.mean_split().count()))
           - log2(static_cast<double>(read_delay .mean_split().count())); }


  bool operator<<(T const &x)
    { interceptor.intercept();
      write_delay.start(); await_write(); write_delay.stop();
      if (!writable()) return false;
      xs.push(qe(stopwatch::now(), x));
      return true; }


  bool operator>>(T &x)
    { interceptor.intercept();
      read_delay.start(); await_read(); read_delay.stop();
      if (!readable()) return false;
      x = std::get<1>(xs.front());
      latency << stopwatch::now() - std::get<0>(xs.front());
      xs.pop();
      return true; }
};


template<class T>
static std::ostream &operator<<(std::ostream &s, pipe<T> &p)
{
  return s << "pipe["
           << (p.readable() ? "R" : "r")
           << (p.writable() ? "W" : p.closed() ? "#" : "w")
           << " n=" << p.latency.n_splits
           << " c=" << p.size() << "/" << p.capacity
           << " λ=" << p.latency.mean_split()
           << " σ=" << p.pressure() << "]";
}


}


#include "../end.hh"

#endif
