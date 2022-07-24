#include <chrono>
#include <iostream>
#include <thread>


#include "../c/fabric.hh"

#include "../c/begin.hh"


using namespace std;
using namespace std::literals;
using namespace tau::fabric;

namespace tc = tau::coro;
namespace tf = tau::fabric;


void try_streams()
{
  cout << "trying streams" << endl;
  tc::null_monitor m;
  tf::null_interceptor i;

  tf::pipe<int> s1(8, i);
  tf::pipe<int> s2(8, i);
  tf::pipe<int> s3(8, i);

  tf::stopwatch clk; clk.start();

  tc::coro<int> f1("f1", m, [&]() {
    cout << "f1 starting" << endl;
    for (int i = 0;; ++i)
    {
      cout << clk.elapsed() << ": writing to s1" << endl;
      if (!(s1 << i)) { cout << "s1 rejected " << i << endl; break; }
      cout << clk.elapsed() << ": wrote to s1" << endl;
      std::this_thread::sleep_for(10ms);
    }
    s1.close();
    return 0;
  });

  tc::coro<int> f2("f2", m, [&]() {
    cout << "f2 starting" << endl;
    int t = 0;
    int x;
    cout << clk.elapsed() << ": reading from s1" << endl;
    while (s1 >> x)
    {
      cout << clk.elapsed() << ": read from s1" << endl;
      t += x;
      cout << clk.elapsed() << ": writing to s2" << endl;
      if (!(s2 << t)) { cout << "s2 rejected " << t << endl; break; }
      cout << clk.elapsed() << ": wrote to s2" << endl;
      std::this_thread::sleep_for(20ms);
      cout << clk.elapsed() << ": reading from s1" << endl;
    }
    s1.close();
    s2.close();
    return 0;
  });

  tc::coro<int> f3("f3", m, [&]() {
    cout << "f3 starting" << endl;
    for (int i = 0; i < 100; ++i)
    {
      int x;
      cout << clk.elapsed() << ": reading from s2" << endl;
      if (!(s2 >> x)) { cout << "s2 gave no input" << endl; break; }
      cout << clk.elapsed() << ": read from s2" << endl;
      cout << clk.elapsed() << ": writing to s3" << endl;
      if (!(s3 << x)) { cout << "s3 rejected " << x << endl; break; }
      cout << clk.elapsed() << ": wrote to s3" << endl;
      std::this_thread::sleep_for(40ms);
    }
    s2.close();
    s3.close();
    return 0;
  });

  tc::coro<int> f4("f4", m, [&]() {
    cout << "f4 starting" << endl;
    int x;
    cout << clk.elapsed() << ": reading from s3" << endl;
    while (s3 >> x)
    {
      cout << clk.elapsed() << ": read from s3" << endl;
      cout << x << endl;
      cout << clk.elapsed() << ": reading from s3" << endl;
    }
    s3.close();
    return 0;
  });

  cout << "s1: " << s1 << endl;
  cout << "s2: " << s2 << endl;

  while (!f1.done() || !f2.done() || !f3.done() || !f4.done())
  {
    cout << "s1: " << s1 << endl;
    cout << "s2: " << s2 << endl;
    cout << "s3: " << s3 << endl;
    cout << f1.done() << ", " << f2.done() << ", "
         << f3.done() << ", " << f4.done() << endl;

    if (!f1.done()) f1();
    if (!f2.done()) f2();
    if (!f3.done()) f3();
    if (!f4.done()) f4();
  }

  cout << "s1: " << s1 << endl;
  cout << "s2: " << s2 << endl;
  cout << "all done" << endl;
}


int main()
{
  try_streams();
  return 0;
}


#include "../c/end.hh"
