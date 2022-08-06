#include <cstdint>
#include <chrono>
#include <iostream>
#include <thread>

#include "../tau.hh"

#include "../tau/module/begin.hh"

namespace t::scheduler_basics
{

using namespace std;
using namespace std::literals;

using namespace tau;


int main()
{
  tau::flux::Λ c;

  let p1 = c.create_pipe(8);
  let p2 = c.create_pipe(12);
  let p3 = c.create_pipe(16);


  let t1 = c.create_task([&]() {
    for (i64 i = 0;; ++i)
      if (!c.write(p1, u9(i))) { cout << "s1 rejected " << i << endl; break; }
    c.close(p1);
    cout << "t1 returning" << endl;
    return 0;
  });

  let t2 = c.create_task([&]() {
    i64 t = 0;
    while (c.has_next(p1))
    {
      t += Sc<i64>(c.next(p1));
      if (!c.write(p2, u9(t))) { cout << "s2 rejected " << t << endl; break; }
      c.sleep(1ms);
    }
    c.close(p1);
    c.close(p2);
    cout << "t2 returning" << endl;
    return 0;
  });

  let t3 = c.create_task([&]() {
    for (int i = 0; c.has_next(p2) && i < 100; ++i)
    {
      let x = c.next(p2);
      if (!c.write(p3, x)) { cout << "s3 rejected " << x << endl; break; }
      c.sleep(2ms);
    }
    c.close(p2);
    c.close(p3);
    cout << "t3 returning" << endl;
    return 0;
  });

  let t4 = c.create_task([&]() {
    i64 last;
    while (c.has_next(p3)) cout << c.uptime() << ": " << (last = Sc<i64>(c.next(p3))) << endl;
    c.close(p3);

    cout << "final value: " << last << endl;
    cout << "t4 returning" << endl;
    return last;
  });


  cout << "starting the scheduler loop" << endl;
  c.run_until(util::ΣΘΔ::now() + 20s);

  cout << c << endl;

  cout << c.uptime() << ": loop end; destroying pipes" << endl;

  c.destroy_pipe(p1);
  c.destroy_pipe(p2);
  c.destroy_pipe(p3);

  cout << "t1 returned " << c.collect_task(t1) << endl;
  cout << "t2 returned " << c.collect_task(t2) << endl;
  cout << "t3 returned " << c.collect_task(t3) << endl;
  cout << "t4 returned " << c.collect_task(t4) << endl;

  cout << "all done " << c << endl;


  return 0;
}

}

#include "../tau/module/end.hh"
