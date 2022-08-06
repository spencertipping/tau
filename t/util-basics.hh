#include <chrono>
#include <iostream>
#include <thread>

#include "unistd.h"


#ifndef QUICK
#define QUICK 0
#endif


#include "../tau.hh"


#include "../tau/module/begin.hh"

namespace t::util_basics
{

using namespace std;
using namespace std::literals;
using namespace tau::util;
using tau::operator<<;


void try_stopwatch()
{
  ΣΘΔ s;
  cout << "empty stopwatch: " << s << endl;

  s.start();
  std::this_thread::sleep_for(100ms);
  cout << "running stopwatch: " << s << endl;
  s.stop();
  cout << "stopped stopwatch: " << s << endl;
}


void bench_stopwatch()
{
  if (!QUICK)
  {
    ΣΘΔ t;
    t.start();

    ΣΘΔ s;
    for (unsigned i = 0; i < 1000000; ++i)
    {
      s.start();
      s.stop();
    }

    t.stop();

    cout << "all stopwatch observations: " << s << endl;
    cout << "each stopwatch observation: " << t.Σ() / 1000000 << endl;

    ΔΘ z;
    ΣΘΔ u;
    u.start();
    for (unsigned i = 0; i < 1000000; ++i)
    {
      let x = (double) i / 1000000.0;
      z += s.p(x);
    }
    u.stop();

    cout << "total z: " << z << endl;
    cout << "p(x) calculations: " << u << endl;
    cout << "p(x) each: " << u.Σ() / 1000000 << endl;
  }
}


int main()
{
  try_stopwatch();
  bench_stopwatch();
  return 0;
}


}

#include "../tau/module/end.hh"
