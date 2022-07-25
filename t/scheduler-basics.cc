#include <cstdint>
#include <chrono>
#include <iostream>
#include <thread>


#include "../c/utf9.hh"
#include "../c/fabric.hh"

#include "../c/begin.hh"


using namespace std;
using namespace std::literals;
using namespace tau::fabric;
using namespace tau::utf9;

namespace tf = tau::fabric;
namespace t9 = tau::utf9;



int main()
{
  tf::scheduler s;

  let p1 = s.create_pipe();
  let p2 = s.create_pipe();
  let p3 = s.create_pipe();


  let t1 = s.create_task([&]() {
    for (int64_t i = 0;; ++i)
    {
      if (!s.write(p1, t9::val(i))) { cout << "s1 rejected " << i << endl; break; }
    }
    s.close(p1);
    return 0;
  });


  let t2 = s.create_task([&]() {
    int64_t t = 0;
    while (s.has_next(p1))
    {
      t += static_cast<int64_t>(s.next(p1));
      if (!s.write(p2, t9::val(t))) { cout << "s2 rejected " << t << endl; break; }
      s.sleep(1ms);
    }
    s.close(p1);
    s.close(p2);
    return 0;
  });


  let t3 = s.create_task([&]() {
    for (int i = 0; s.has_next(p2) && i < 100; ++i)
    {
      let x = s.next(p2);
      if (!s.write(p3, x)) { cout << "s3 rejected " << x << endl; break; }
      s.sleep(2ms);
    }
    s.close(p2);
    s.close(p3);
    return 0;
  });


  let t4 = s.create_task([&]() {
    while (s.has_next(p3))
    {
      cout << s.next(p3) << endl;
    }
    s.close(p3);
    return 0;
  });


  cout << "starting the scheduler loop" << endl;
  while (s.runnable_tasks())
  {
    //cout << s.uptime() << ": " << s.runnable_tasks() << " runnable task(s)" << endl;

    let t = s.next_runnable();
    //cout << s.uptime() << ": next task: " << t << endl;
    if (t) s.run_queue.pop();

    //cout << s.uptime() << ": yielding into " << t << endl;
    s.yield_into(t);
    //cout << s.uptime() << ": returned from " << t << endl;

    let d = s.next_deadline();
    //cout << s.uptime() << ": next deadline: " << (d - stopwatch::now()) << endl;
    if (d.time_since_epoch() > 0s)
    {
      std::this_thread::sleep_until(d);
      //cout << s.uptime() << ": awake" << endl;
    }

    //cout << s.uptime() << ": awakening sleepers..." << endl;
    s.awaken_sleepers();
    //cout << s.uptime() << ": runnable tasks = " << s.runnable_tasks() << endl;
  }


  // TODO: why do we get fewer than 100 entries out of the pipe?
  // We're exiting too early.


  cout << "p1 = " << s.pipe(p1) << endl;
  cout << "p2 = " << s.pipe(p2) << endl;
  cout << "p3 = " << s.pipe(p3) << endl;


  s.close(p1);
  s.close(p2);
  s.close(p3);
  s.run_until_deadline();


  cout << s.uptime() << ": loop end; destroying pipes" << endl;

  s.destroy_pipe(p1);
  s.destroy_pipe(p2);
  s.destroy_pipe(p3);

  s.destroy_task(t1);
  s.destroy_task(t2);
  s.destroy_task(t3);
  s.destroy_task(t4);


  cout << "all done" << endl;


  return 0;
}



#include "../c/end.hh"
