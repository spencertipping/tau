#ifndef tau_fabric_scheduler_h
#define tau_fabric_scheduler_h


#include <chrono>
#include <functional>
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


// FIXME: task results should be something more interesting, probably
typedef int                          task_result;
typedef tc::coro<task_result>        task_coro;
typedef std::function<task_result()> task_fn;

typedef t9::val                      pipe_val;
typedef pipe<t9::val>                scheduled_pipe;


struct runnable_state
{
  enum state
  {
    RUNNABLE,
    SLEEP,
    READ_BLOCKED,
    WRITE_BLOCKED
  };

  state s;
  union
  {
    stopwatch::tp deadline;
    pipe_id       pipe;
  };

  bool is_timed() const { return s == SLEEP; }
};


struct scheduled_task
{
  stopwatch      monitor;
  task_coro      coro;
  runnable_state state;

  scheduled_task() : state{runnable_state::RUNNABLE} {}
  scheduled_task(task_id i, task_fn &f)
    : coro(task_coro(f)),
      state{runnable_state::RUNNABLE} {}


  scheduled_task &operator=(scheduled_task &&t)
    { monitor = t.monitor;
      coro    = std::move(t.coro);
      state   = t.state;
      return *this; }


  stopwatch::span p(double p) const { return monitor.p(p); }

  bool operator<(scheduled_task const &t) const;
};


struct deadline_schedule
{
  scheduler &s;
  bool operator()(task_id a, task_id b) const;
};


struct scheduler
{
  template<class S>
  using pq = std::priority_queue<task_id, std::vector<task_id>, S>;


  std::unordered_map<task_id, scheduled_task> tasks;
  std::unordered_map<pipe_id, scheduled_pipe> pipes;
  std::unordered_map<pipe_id, task_id>        read_blocks;
  std::unordered_map<pipe_id, task_id>        write_blocks;

  deadline_schedule ds;

  std::queue<task_id>   run_queue;
  pq<deadline_schedule> sleep_queue;

  task_id current_task;
  task_id next_task_id;
  pipe_id next_pipe_id;
  stopwatch::tp const ctime;


  scheduler() : ds          {*this},
                sleep_queue (ds),
                current_task{0},
                next_task_id{1},
                next_pipe_id{1},
                ctime       (stopwatch::now()) {}


  stopwatch::span uptime() const { return stopwatch::now() - ctime; }


  void     close   (pipe_id);
  bool     has_next(pipe_id);
  pipe_val next    (pipe_id);
  bool     write   (pipe_id, pipe_val const &);


  size_t runnable_tasks() const { return run_queue.size(); }

  stopwatch::tp next_deadline() const
    { return sleep_queue.size()
        ? tasks.at(sleep_queue.top()).state.deadline
        : stopwatch::tp{0ns}; }


  void awaken_sleepers()
    { let n = stopwatch::now();
      while (sleep_queue.size() && tasks.at(sleep_queue.top()).state.deadline <= n)
      { let t = sleep_queue.top();
        sleep_queue.pop();
        wake(t); } }


  task_id next_runnable() { return run_queue.size() ? run_queue.front() : 0; }
  void run_until_deadline()
    { task_id i;
      while (i = next_runnable())
      { run_queue.pop();
        yield_into(i);
        awaken_sleepers(); } }

  void yield_into(task_id i)
    { assert(!current_task);
      assert(tasks.at(i).state.s == runnable_state::RUNNABLE);
      current_task = i;
      tasks.at(i).coro();
      current_task = 0; }


  pipe_id create_pipe(size_t capacity = 64)
    { let k = next_pipe_id++;
      pipes[k] = scheduled_pipe(capacity);
      return k; }

  task_id create_task(task_fn f)
    { let k = next_task_id++;
      tasks[k] = scheduled_task(k, f);
      wake(k);
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

  void sleep(stopwatch::span s)
    { let   d = stopwatch::now() + s;
      auto &t = tasks.at(current_task);
      t.state.s        = runnable_state::SLEEP;
      t.state.deadline = d;
      sleep_queue.push(current_task);
      tc::yield(); }
};


bool deadline_schedule::operator()(task_id a, task_id b) const
{ return s.tasks.at(a).state.deadline < s.tasks.at(b).state.deadline; }


bool scheduled_task::operator<(scheduled_task const &t) const
{
  if (state.s != t.state.s) return state.s < t.state.s;
  switch (state.s)
  {
  case runnable_state::SLEEP:
    return state.deadline < t.state.deadline;

  default:
    return false;
  }
}


void scheduler::wake(task_id i)
{
  auto &t = tasks.at(i);
  assert(!t.coro.done());
  t.state.s = runnable_state::RUNNABLE;
  run_queue.push(i);
}


void scheduler::close(pipe_id i)
{
  auto &p = pipes.at(i);
  if (!p.closed())
  {
    read_wake(i);
    write_wake(i);
    p.close();
  }
}


bool scheduler::has_next(pipe_id i)
{
  auto &p = pipes.at(i);
  if (p.readable()) read_wake(i);
  else
  {
    p.read_delay.start();
    while (!p.readable())
    {
      auto &t = tasks.at(current_task);
      t.state.s    = runnable_state::READ_BLOCKED;
      t.state.pipe = i;
      read_blocks[i] = current_task;
      tc::yield();
    }
    p.read_delay.stop();
  }

  return p.has_next();
}


pipe_val scheduler::next(pipe_id i)
{
  return pipes.at(i).next();
}


bool scheduler::write(pipe_id i, pipe_val const &v)
{
  auto &p = pipes.at(i);
  if (p.writable()) write_wake(i);
  else
  {
    p.write_delay.start();
    while (!p.writable())
    {
      auto &t = tasks.at(current_task);
      t.state.s    = runnable_state::WRITE_BLOCKED;
      t.state.pipe = i;
      write_blocks[i] = current_task;
      tc::yield();
    }
    p.write_delay.stop();
  }

  return p.write(v);
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
