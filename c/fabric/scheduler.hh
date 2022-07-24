#ifndef tau_fabric_scheduler_h
#define tau_fabric_scheduler_h


#include <chrono>
#include <queue>
#include <unordered_map>


#include "../utf9.hh"
#include "../coro.hh"

#include "pipe.hh"
#include "stopwatch.hh"


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


struct task_monitor
{
  template<class T>
  using co = tc::coro<T, task_monitor>;

  stopwatch s;

  template<class T> void init    (co<T> &c) {}
  template<class T> void enter   (co<T> &c) { s.start(); }
  template<class T> void exit    (co<T> &c) { s.stop();  }
  template<class T> void finalize(co<T> &c) {}
};


struct pipe_interceptor
{
  scheduler &s;
  pipe_id    i;

  void intercept_close();
  bool intercept_read();
  bool intercept_write();
};


typedef tc::coro<t9::val, task_monitor> task_coro;
typedef std::function<t9::val()>        task_fn;
typedef pipe<t9::val, pipe_interceptor> scheduled_pipe;


struct runnable_state
{
  enum state
  {
    RUNNABLE,
    PRECISE_SLEEP,
    IMPRECISE_SLEEP,
    READ_BLOCKED,
    WRITE_BLOCKED
  };

  state s;
  union
  {
    stopwatch::tp deadline;
    pipe_id       pipe;
  };
};


struct scheduled_task
{
  task_monitor   monitor;
  task_coro      coro;
  runnable_state state;
  double         prio;

  scheduled_task(task_id i, task_fn &f)
    : monitor(task_monitor()),
      coro(task_coro(std::to_string(i), monitor, f)),
      state{runnable_state::RUNNABLE} {}

  bool operator<(scheduled_task const &t) const
    {
      // TODO: scheduling priority logic
    }
};


struct scheduler
{
  std::unordered_map<task_id, scheduled_task> tasks;
  std::unordered_map<pipe_id, scheduled_pipe> pipes;
  std::unordered_map<pipe_id, task_id>        read_blocks;
  std::unordered_map<pipe_id, task_id>        write_blocks;

  task_id current_task;
  task_id next_task_id {1};
  pipe_id next_pipe_id {1};


  void run_until_deadline()
    {
    }


  void step()
    {
    }


  pipe_id create_pipe(size_t capacity)
    { let k = next_pipe_id++;
      pipes.emplace(k, scheduled_pipe(capacity, pipe_interceptor{*this, k}));
      return k; }

  task_id create_task(task_fn f)
    { let k = next_task_id++;
      tasks.emplace(k, scheduled_task(k, f));
      return k; }

  void destroy_pipe(pipe_id i)
    { assert(!read_blocks.contains(i));
      assert(!write_blocks.contains(i));
      pipes.erase(i); }

  void destroy_task(task_id i)
    { assert(i != current_task);
      tasks.erase(i); }

  scheduled_pipe const &pipe(pipe_id i)     const { return pipes.at(i); }
  scheduled_task const &task(task_id i = 0) const { return tasks.at(i ? i : current_task); }


  void read_wake(pipe_id);
  void write_wake(pipe_id);
  void wake(task_id);

  void intercept_close(pipe_id i);
  void intercept_read(pipe_id i);
  void intercept_write(pipe_id i);

  void sleep(stopwatch::span s)
    { let   d = stopwatch::now() + s;
      auto &t = tasks[current_task];
      t.state.s        = runnable_state::IMPRECISE_SLEEP;
      t.state.deadline = d;
      tc::yield(); }

  void sleep(stopwatch::tp d)
    { auto &t = tasks[current_task];
      t.state.s        = runnable_state::PRECISE_SLEEP;
      t.state.deadline = d;
      tc::yield(); }
};


void pipe_interceptor::intercept_close() { s.intercept_close(i); }
bool pipe_interceptor::intercept_read()  { s.intercept_read(i);  return true; }
bool pipe_interceptor::intercept_write() { s.intercept_write(i); return true; }


void scheduler::intercept_read(pipe_id i)
{
  auto &p = pipes.at(i);
  if (!p.readable())
  {
    p.read_delay.start();
    auto &t = tasks[current_task];
    t.state.s    = runnable_state::READ_BLOCKED;
    t.state.pipe = i;
    read_blocks[i] = current_task;
    tc::yield();
  }
  else
    read_wake(i);
}

void scheduler::intercept_write(pipe_id i)
{
  auto &p = pipes.at(i);
  if (!p.writable())
  {
    p.write_delay.start();
    auto &t = tasks[current_task];
    t.state.s    = runnable_state::WRITE_BLOCKED;
    t.state.pipe = i;
    write_blocks[i] = current_task;
    tc::yield();
  }
  else
    write_wake(i);
}


void scheduler::read_wake(pipe_id i)
{
  if (!read_blocks.contains(i)) return;
  let t = read_blocks[i];
  read_blocks.erase(i);
  wake(t);
}

void scheduler::write_wake(pipe_id i)
{
  if (!write_blocks.contains(i)) return;
  let t = write_blocks[i];
  write_blocks.erase(i);
  wake(t);
}


}


#include "../end.hh"


#endif
