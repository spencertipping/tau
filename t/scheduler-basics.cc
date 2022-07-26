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

  let p1 = s.create_pipe(8);
  let p2 = s.create_pipe(12);
  let p3 = s.create_pipe(16);


  let t1 = s.create_task([&]() {
    for (int64_t i = 0;; ++i)
      if (!s.write(p1, t9::val(i))) { cout << "s1 rejected " << i << endl; break; }
    s.close(p1);
    cout << "t1 returning" << endl;
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
    cout << "t2 returning" << endl;
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
    cout << "t3 returning" << endl;
    return 0;
  });

  let t4 = s.create_task([&]() {
    int64_t last;
    while (s.has_next(p3)) cout << s.uptime() << ": " << (last = s.next(p3)) << endl;
    s.close(p3);

    cout << "final value: " << last << endl;
    cout << "t4 returning" << endl;
    return last;
  });


  cout << "starting the scheduler loop" << endl;
  s.run_until(stopwatch::now() + 20s);

  cout << s << endl;

  cout << s.uptime() << ": loop end; destroying pipes" << endl;

  s.destroy_pipe(p1);
  s.destroy_pipe(p2);
  s.destroy_pipe(p3);

  cout << "t1 returned " << s.collect_task(t1) << endl;
  cout << "t2 returned " << s.collect_task(t2) << endl;
  cout << "t3 returned " << s.collect_task(t3) << endl;
  cout << "t4 returned " << s.collect_task(t4) << endl;

  cout << "all done " << s << endl;


  return 0;
}



#include "../c/end.hh"
