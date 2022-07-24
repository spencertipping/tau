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


template<class T, class I> struct pipe;

namespace
{

struct null_interceptor
{
  void intercept_close() {}
  bool intercept_read()  { return false; }
  bool intercept_write() { return false; }
};

}


template<class T, class I = null_interceptor>
struct pipe
{
  typedef std::pair<stopwatch::tp, T> qe;

  stopwatch      read_delay;
  stopwatch      write_delay;
  stopwatch      latency;
  size_t         capacity;
  std::deque<qe> xs;
  I              interceptor;

  pipe(size_t c_, I const &i_) : capacity(c_), interceptor(i_) { assert(capacity); }


  bool readable() const { return xs.size() > 0; }
  bool writable() const { return xs.size() < capacity; }

  void await_read()  const { while (!closed() && !readable()) tc::yield(); }
  void await_write() const { while (!closed() && !writable()) tc::yield(); }

  operator bool() const { await_read(); return readable(); }

  void   close()        { interceptor.intercept_close(); capacity = 0; }
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


  bool operator<<(T const &x)
    { let n = stopwatch::now();
      if (!interceptor.intercept_write())
      { write_delay.start(); await_write(); write_delay.stop(); }
      if (!writable()) return false;
      xs.push_back(qe(n, x));
      return true; }


  bool operator>>(T &x)
    { if (!interceptor.intercept_read())
      { read_delay.start(); await_read(); read_delay.stop(); }
      if (!readable()) return false;
      x = std::get<1>(xs.front());
      latency << stopwatch::now() - std::get<0>(xs.front());
      xs.pop_front();
      return true; }
};


template<class T>
static std::ostream &operator<<(std::ostream &s, pipe<T> &p)
{
  return s << "pipe["
           << (p.readable() ? "R" : "r")
           << (p.writable() ? "W" : p.closed() ? "#" : "w")
           << " n=" << p.total_written()
           << " c=" << p.size() << "/" << p.capacity
           << " rd=" << p.read_delay
           << " wd=" << p.write_delay
           << " λ=" << p.λ()
           << " σ=" << p.σ() << "]";
}


}


#include "../end.hh"

#endif
