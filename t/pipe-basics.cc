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
  tf::pipe<int> s1(8);
  tf::pipe<int> s2(8);
  tf::pipe<int> s3(8);

  tf::stopwatch clk; clk.start();

  tc::coro<int> f1([&]() {
    cout << "f1 starting" << endl;
    for (int i = 0;; ++i)
    {
      cout << clk.elapsed() << ": writing to s1" << endl;
      if (!s1.write(i)) { cout << "s1 rejected " << i << endl; break; }
      cout << clk.elapsed() << ": wrote to s1" << endl;
      std::this_thread::sleep_for(10ms);
    }
    s1.close();
    return 0;
  });

  tc::coro<int> f2([&]() {
    cout << "f2 starting" << endl;
    int t = 0;
    cout << clk.elapsed() << ": reading from s1" << endl;
    while (s1.has_next())
    {
      cout << clk.elapsed() << ": read from s1" << endl;
      t += s1.next();
      cout << clk.elapsed() << ": writing to s2" << endl;
      if (!s2.write(t)) { cout << "s2 rejected " << t << endl; break; }
      cout << clk.elapsed() << ": wrote to s2" << endl;
      std::this_thread::sleep_for(20ms);
      cout << clk.elapsed() << ": reading from s1" << endl;
    }
    s1.close();
    s2.close();
    return 0;
  });

  tc::coro<int> f3([&]() {
    cout << "f3 starting" << endl;
    for (int i = 0; i < 100; ++i)
    {
      cout << clk.elapsed() << ": reading from s2" << endl;
      if (!s2.has_next()) { cout << "s2 gave no input" << endl; break; }
      let x = s2.next();
      cout << clk.elapsed() << ": read from s2" << endl;
      cout << clk.elapsed() << ": writing to s3" << endl;
      if (!s3.write(x)) { cout << "s3 rejected " << x << endl; break; }
      cout << clk.elapsed() << ": wrote to s3" << endl;
      std::this_thread::sleep_for(40ms);
    }
    s2.close();
    s3.close();
    return 0;
  });

  tc::coro<int> f4([&]() {
    cout << "f4 starting" << endl;
    cout << clk.elapsed() << ": reading from s3" << endl;
    while (s3.has_next())
    {
      cout << clk.elapsed() << ": read from s3" << endl;
      cout << s3.next() << endl;
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
