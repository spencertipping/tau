#ifndef tau_fabric_scheduler_h
#define tau_fabric_scheduler_h


#include <chrono>
#include <functional>
#include <iostream>
#include <queue>
#include <thread>
#include <unordered_map>
#include <unordered_set>


#include "../utf9.hh"
#include "../coro.hh"

#include "pipe.hh"
#include "stopwatch.hh"
#include "scheduler-types.hh"


#include "../begin.hh"


namespace tau::fabric
{

using namespace std::literals;

namespace tc = tau::coro;


void thread_sleep_until(stopwatch::tp t) { std::this_thread::sleep_until(t); }


std::ostream &operator<<(std::ostream &s, scheduler const &x);

struct scheduler
{
  template<class S>
  using pq = std::priority_queue<task_id, std::vector<task_id>, S>;

  typedef void(*sleep_until_fn)(stopwatch::tp);


  // invariant: every task is in read_blocks, write_blocks, run_queue, or
  // sleep_queue
  std::unordered_map<task_id, scheduled_task> tasks;
  std::unordered_map<pipe_id, scheduled_pipe> pipes;
  std::unordered_map<pipe_id, task_id>        read_blocks;
  std::unordered_map<pipe_id, task_id>        write_blocks;
  std::unordered_set<task_id>                 done_tasks;

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

  pipe_id create_pipe(size_t capacity = 64)
    { let k = next_pipe_id++;
      pipes[k] = scheduled_pipe(capacity);
      return k; }

  task_id create_task(task_fn f)
    { let k = next_task_id++;
      tasks[k] = scheduled_task(f);
      wake(k);
      return k; }

  void destroy_pipe(pipe_id i)
    { assert(pipe(i).closed());
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

  scheduled_pipe const &pipe(pipe_id i)     const { return pipes.at(i); }
  scheduled_task const &task(task_id i = 0) const { return tasks.at(i ? i : current_task); }


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
{ return s.tasks.at(a).deadline < s.tasks.at(b).deadline; }


void scheduler::wake(task_id i)
{
  auto &t = tasks.at(i);
  assert(!t.coro.done());
  t.state = RUNNABLE;
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


pipe_val scheduler::next(pipe_id i)
{
  auto &p = pipes.at(i);
  assert(p.readable());
  return p.next();
}


bool scheduler::write(pipe_id i, pipe_val const &v)
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


std::ostream &operator<<(std::ostream &s, scheduler const &x)
{
  s << "scheduler " << x.uptime() << std::endl;
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


#include "../end.hh"


#endif
