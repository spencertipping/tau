#include "utf9-basics.hh"
#include "coro-basics.hh"
#include "scheduler-basics.hh"


#include "../tau.hh"


#include "../tau/module/begin.hh"


int main()
{
  t::utf9_basics::main();
  t::coro_basics::main();
  t::scheduler_basics::main();
  return 0;
}


void disaster_scratch()
{
  using namespace tau;
  using namespace tau::util;
  using namespace std;

  let upper = 1048576;

  o9 o4;
  u9 v4 = u9t(upper);
  for (int64_t i = 0; i < upper; ++i) v4 << u9(i);

  {
    let start = stopwatch::now();
    o4 << v4;
    let end = stopwatch::now();
    cout << upper << " write output: " << end - start << endl;
  }

  let start = stopwatch::now();
  uint64_t t = 0;
  for (auto const &x : v4)
    t += static_cast<int64_t>(x);
  let end   = stopwatch::now();
  cout << "isum: " << t << ": " << end - start << endl;
}


#include "../tau/module/end.hh"
