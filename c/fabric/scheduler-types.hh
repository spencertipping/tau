#ifndef tau_fabric_scheduler_types_h
#define tau_fabric_scheduler_types_h


#include <chrono>
#include <iostream>


#include "../utf9.hh"
#include "../coro.hh"

#include "pipe.hh"


#include "../begin.hh"


namespace tau::fabric
{

using namespace std::literals;

namespace t9 = tau::utf9;
namespace tc = tau::coro;


struct scheduler;
struct scheduled_task;


typedef uint64_t task_id;
typedef uint64_t pipe_id;


// FIXME: task results should be something more interesting, probably
typedef int                          task_result;
typedef tc::coro<task_result>        task_coro;
typedef std::function<task_result()> task_fn;

typedef t9::val                      pipe_val;
typedef pipe<t9::val>                scheduled_pipe;


enum task_state
{
  RUNNABLE,
  SLEEPING,
  READ_BLOCKED,
  WRITE_BLOCKED,
  DONE
};


struct scheduled_task
{
  stopwatch     monitor;
  task_coro     coro;
  stopwatch::tp deadline;
  task_state    state;

  scheduled_task() {}
  scheduled_task(task_fn &f)
    : coro(task_coro(f)), state{RUNNABLE} {}


  scheduled_task &operator=(scheduled_task &&t)
    { monitor  = t.monitor;
      coro     = std::move(t.coro);
      deadline = t.deadline;
      state    = t.state;
      return *this; }

  stopwatch::span p(double p) const { return monitor.p(p); }

  bool operator<(scheduled_task const &t) const;
};


struct deadline_schedule
{
  scheduler &s;
  bool operator()(task_id a, task_id b) const;
};


std::ostream &operator<<(std::ostream &s, task_state t)
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


std::ostream &operator<<(std::ostream &s, scheduled_task const &t)
{
  s << "task[" << t.state;
  if (t.state == SLEEPING) s << " d=" << (t.deadline - stopwatch::now());
  return s << " " << t.monitor << "]";
}


}


#include "../end.hh"


#endif
