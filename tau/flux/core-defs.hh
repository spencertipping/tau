#ifndef tau_flux_core_defs_h
#define tau_flux_core_defs_h


#include <chrono>
#include <iostream>


#include "../utf9.hh"

#include "init.hh"
#include "coro.hh"
#include "pipe.hh"


#include "../module/begin.hh"

namespace tau::flux
{


struct core;
struct task;


typedef uint64_t task_id;
typedef uint64_t pipe_id;

// FIXME: task results should be something more interesting, probably
typedef int                          task_result;
typedef coro::coro<task_result>      task_coro;
typedef std::function<task_result()> task_fn;

typedef u9                           pipe_val;  // FIXME: shared_ptr
typedef pipe<pipe_val>               core_pipe;
typedef task                         core_task;


struct deadline_schedule
{
  core &c;
  bool operator()(task_id a, task_id b) const;
};


}

#include "../module/end.hh"


#endif
