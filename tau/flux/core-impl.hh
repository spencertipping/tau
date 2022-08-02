#ifndef tau_flux_core_impl_h
#define tau_flux_core_impl_h


#include <chrono>
#include <functional>
#include <iostream>
#include <queue>
#include <thread>
#include <unordered_map>
#include <unordered_set>


#include "../utf9.hh"
#include "../util/stopwatch.hh"

#include "init.hh"
#include "coro.hh"
#include "pipe.hh"
#include "core-defs.hh"
#include "core-task.hh"


#include "../module/begin.hh"

namespace tau::flux
{

using namespace std::literals;
namespace tc = tau::flux::coro;


void thread_sleep_until(stopwatch::tp t) { std::this_thread::sleep_until(t); }


struct core
{
  template<class S>
  using pq = std::priority_queue<task_id, std::vector<task_id>, S>;

  typedef void(*sleep_until_fn)(stopwatch::tp);


  // TODO: it should be possible to wait on a union of conditions
  // (e.g. pipe readable | 100ms are up)

  // TODO: split this class into several sub-pieces:
  // + pipe sleep/wake system
  // + deadline scheduler
  // + deadline risk management
  // + quantum limit (for more precise time slicing)
  // + run queue


  // invariant: every task is in read_blocks, write_blocks, run_queue, or
  // sleep_queue
  std::unordered_map<task_id, core_task> tasks;
  std::unordered_map<pipe_id, core_pipe> pipes;
  std::unordered_map<pipe_id, task_id>   read_blocks;
  std::unordered_map<pipe_id, task_id>   write_blocks;
  std::unordered_set<task_id>            done_tasks;

  deadline_schedule ds;

  std::queue<task_id>   run_queue;
  pq<deadline_schedule> sleep_queue;

  task_id current_task;
  task_id next_task_id;
  pipe_id next_pipe_id;
  stopwatch::tp const ctime;


  core() : ds          {*this},
           sleep_queue (ds),
           current_task{0},
           next_task_id{1},
           next_pipe_id{1},
           ctime       (stopwatch::now()) {}


  stopwatch::span uptime() const { return stopwatch::now() - ctime; }

  pipe_id create_pipe(size_t capacity = 64)
    { let k = next_pipe_id++;
      pipes[k] = core_pipe(capacity);
      return k; }

  task_id create_task(task_fn const &f)
    { let k = next_task_id++;
      tasks[k] = core_task(f);
      wake(k);
      return k; }

  void destroy_pipe(pipe_id i)
    { assert(pipes.at(i).closed());
      pipes.erase(i); }

  void destroy_task(task_id i)
    { assert(i != current_task);
      tasks.erase(i); }

  task_result collect_task(task_id i)
    { auto &t = tasks.at(i);
      assert(t.state == DONE);
      let r = t.coro.result();
      destroy_task(i);
      return r; }


  void     close   (pipe_id);
  bool     has_next(pipe_id);
  pipe_val next    (pipe_id);
  bool     write   (pipe_id, pipe_val const &);


  size_t        runnable_tasks() const { return run_queue.size(); }
  task_id       next_runnable()  const { return run_queue.size() ? run_queue.front() : 0; }
  stopwatch::tp next_deadline()  const { return sleep_queue.size()
      ? tasks.at(sleep_queue.top()).deadline
      : stopwatch::never(); }

  stopwatch::tp run_until_deadline()
    { for (task_id i; i = next_runnable();)
      { run_queue.pop();
        run(i);
        advance_time(); }
      return next_deadline(); }

  void run_until(stopwatch::tp  limit = stopwatch::forever(),
                 sleep_until_fn sfn   = &thread_sleep_until)
    { while (stopwatch::now() < limit && runnable_tasks())
      { let d = run_until_deadline();
        if (d != stopwatch::never()) (*sfn)(d);
        advance_time(); } }

  void run(task_id i)
    { assert(!current_task);
      current_task = i;
      auto &t = tasks.at(i);
      t.monitor.start();
      t.coro();
      t.monitor.stop();
      if (t.coro.done()) { t.state = DONE; done_tasks.insert(i); }
      current_task = 0; }

  void advance_time(stopwatch::tp n = stopwatch::now())
    { while (sleep_queue.size() && tasks.at(sleep_queue.top()).deadline <= n)
      { let t = sleep_queue.top();
        sleep_queue.pop();
        wake(t); } }


  void read_wake(pipe_id);
  void write_wake(pipe_id);
  void wake(task_id);

  void sleep(stopwatch::span s)
    { auto &t = tasks.at(current_task);
      t.deadline = stopwatch::now() + s;
      t.state    = SLEEPING;
      sleep_queue.push(current_task);
      tc::yield(); }

  void yield()
    { wake(current_task);
      tc::yield(); }
};


bool deadline_schedule::operator()(task_id a, task_id b) const
{ return c.tasks.at(a).deadline < c.tasks.at(b).deadline; }


void core::wake(task_id i)
{
  auto &t = tasks.at(i);
  assert(!t.coro.done());
  t.state = RUNNABLE;
  run_queue.push(i);
}


void core::close(pipe_id i)
{
  auto &p = pipes.at(i);
  if (!p.closed())
  {
    read_wake(i);
    write_wake(i);
    p.close();
  }
}


bool core::has_next(pipe_id i)
{
  auto &p = pipes.at(i);
  p.read_delay.start();
  if (p.readable()) write_wake(i);
  else
    while (!p.readable() && !p.closed())
    {
      read_blocks[i] = current_task;
      tasks.at(current_task).state = READ_BLOCKED;
      tc::yield();
    }
  p.read_delay.stop();

  return p.has_next();
}


pipe_val core::next(pipe_id i)
{
  auto &p = pipes.at(i);
  assert(p.readable());
  return p.next();
}


bool core::write(pipe_id i, pipe_val const &v)
{
  auto &p = pipes.at(i);
  p.write_delay.start();
  if (p.writable()) read_wake(i);
  else
    while (!p.writable() && !p.closed())
    {
      write_blocks[i] = current_task;
      tasks.at(current_task).state = WRITE_BLOCKED;
      tc::yield();
    }
  p.write_delay.stop();

  return p.write(v);
}


void core::read_wake(pipe_id i)
{
  if (!read_blocks.contains(i)) return;
  let t = read_blocks[i];
  read_blocks.erase(i);
  wake(t);
}

void core::write_wake(pipe_id i)
{
  if (!write_blocks.contains(i)) return;
  let t = write_blocks[i];
  write_blocks.erase(i);
  wake(t);
}


std::ostream &operator<<(std::ostream &s, core const &x)
{
  s << "flux::core[" << x.uptime() << "]" << std::endl;
  for (let &t : x.tasks) s << "  " << t.first << " " << t.second << std::endl;
  for (let &[k, v] : x.pipes)
  {
    s << "  " << k << " ";
    if (x.read_blocks.contains(k))  s << "R" << x.read_blocks.at(k)  << " ";
    if (x.write_blocks.contains(k)) s << "W" << x.write_blocks.at(k) << " ";
    s << v << std::endl;
  }
  return s;
}


}

#include "../module/end.hh"


#endif
