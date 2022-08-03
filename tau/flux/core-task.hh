#ifndef tau_flux_core_task_h
#define tau_flux_core_task_h


#include <iostream>


#include "../util/stopwatch.hh"

#include "init.hh"
#include "core-defs.hh"


#include "../module/begin.hh"

namespace tau::flux
{


enum λs
{
  RUNNABLE,
  SLEEPING,
  READ_BLOCKED,
  WRITE_BLOCKED,
  DONE
};


struct λ
{
  stopwatch     monitor;
  λc            coro;
  stopwatch::tp deadline;
  λs            state;

  λ() {}
  λ(λf const &f) : coro(λc(f)), state{RUNNABLE} {}


  λ &operator=(λ &&t)
    { monitor  = t.monitor;
      coro     = std::move(t.coro);
      deadline = t.deadline;
      state    = t.state;
      return *this; }

  stopwatch::span p(double p) const { return monitor.p(p); }

  bool operator<(λ const &t) const;
};


std::ostream &operator<<(std::ostream &s, λs const &t)
{
  switch (t)
  {
  case RUNNABLE:      return s << "R";
  case SLEEPING:      return s << "S";
  case READ_BLOCKED:  return s << "I";
  case WRITE_BLOCKED: return s << "O";
  case DONE:          return s << "Z";
  default:            return s << "BOGUS " << static_cast<int64_t>(t);
  }
}


std::ostream &operator<<(std::ostream &s, λ const &t)
{
  s << "task[" << t.state;
  if (t.state == SLEEPING) s << " d=" << (t.deadline - stopwatch::now());
  return s << " " << t.monitor << "]";
}


}

#include "../module/end.hh"


#endif
