#ifndef tau_flux_λ_h
#define tau_flux_λ_h


#include <chrono>
#include <functional>
#include <iostream>
#include <queue>
#include <thread>
#include <unordered_map>
#include <unordered_set>


#include "../types.hh"
#include "../utf9.hh"
#include "../util/stopwatch.hh"

#include "init.hh"
#include "coro.hh"
#include "pipe.hh"
#include "defs.hh"
#include "lambda-defs.hh"


#include "../module/begin.hh"

namespace tau::flux
{

using namespace std::literals;
namespace tc = tau::flux::coro;


void thread_sleep_until(stopwatch::tp t) { std::this_thread::sleep_until(t); }


// TODO: convert this to a task manager that is consumed by Ψ, not the user
// TODO: remove pipe management from this
struct Λ
{
  typedef void(*sleep_until_fn)(stopwatch::tp);


  // TODO: split this class into several sub-pieces:
  // + pipe sleep/wake system
  // + deadline scheduler
  // + deadline risk management
  // + quantum limit (for more precise time slicing)
  // + run queue


  // invariant: every task is in read_blocks, write_blocks, run_queue, or
  // sleep_queue
  M<λi, λ>  tasks;
  M<ψi, ψ>  pipes;
  M<ψi, λi> read_blocks;
  M<ψi, λi> write_blocks;
  S<λi>     done_tasks;

  deadline_schedule ds;

  Q<λi>                     run_queue;
  PQ<λi, deadline_schedule> sleep_queue;

  λi current_task;
  λi next_task_id;
  ψi next_pipe_id;
  Ψi next_boundary_id;

  stopwatch::tp const ctime;


  Λ() : ds          {*this},
        sleep_queue (ds),
        current_task{0},
        next_task_id{1},
        next_pipe_id{1},
        ctime       (stopwatch::now()) {}


  stopwatch::span uptime() const { return stopwatch::now() - ctime; }

  ψi create_pipe(uN capacity = 64)
    { let k = next_pipe_id++;
      pipes[k] = ψ(capacity);
      return k; }

  λi create_task(λf const &f)
    { let k = next_task_id++;
      tasks[k] = λ(f);
      wake(k);
      return k; }

  void destroy_pipe(ψi i)
    { assert(pipes.at(i).closed());
      pipes.erase(i); }

  void destroy_task(λi i)
    { assert(i != current_task);
      tasks.erase(i); }

  λv collect_task(λi i)
    { auto &t = tasks.at(i);
      assert(t.state == DONE);
      let r = t.coro.result();
      destroy_task(i);
      return r; }


  void close   (ψi);
  bool has_next(ψi);
  ψv   next    (ψi);
  bool write   (ψi, ψv const &);


  uN            runnable_tasks() const { return run_queue.size(); }
  λi            next_runnable()  const { return run_queue.size() ? run_queue.front() : 0; }
  stopwatch::tp next_deadline()  const { return sleep_queue.size()
      ? tasks.at(sleep_queue.top()).deadline
      : stopwatch::never(); }

  stopwatch::tp run_until_deadline()
    { for (λi i; i = next_runnable();)
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

  void run(λi i)
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


  void read_wake(λi);
  void write_wake(λi);
  void wake(λi);

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


bool deadline_schedule::operator()(λi a, λi b) const
{ return c.tasks.at(a).deadline < c.tasks.at(b).deadline; }


void Λ::wake(λi i)
{
  auto &t = tasks.at(i);
  assert(!t.coro.done());
  t.state = RUNNABLE;
  run_queue.push(i);
}


void Λ::close(λi i)
{
  auto &p = pipes.at(i);
  if (!p.closed())
  {
    read_wake(i);
    write_wake(i);
    p.close();
  }
}


bool Λ::has_next(ψi i)
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


ψv Λ::next(ψi i)
{
  auto &p = pipes.at(i);
  assert(p.readable());
  return p.next();
}


bool Λ::write(ψi i, ψv const &v)
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


void Λ::read_wake(ψi i)
{
  if (!read_blocks.contains(i)) return;
  let t = read_blocks[i];
  read_blocks.erase(i);
  wake(t);
}

void Λ::write_wake(ψi i)
{
  if (!write_blocks.contains(i)) return;
  let t = write_blocks[i];
  write_blocks.erase(i);
  wake(t);
}


std::ostream &operator<<(std::ostream &s, Λ const &x)
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
