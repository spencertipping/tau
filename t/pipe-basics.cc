#include <chrono>
#include <iostream>
#include <thread>


#include "../c/fabric.hh"

#include "../c/begin.hh"


using namespace std;
using namespace std::literals;

namespace tc = tau::coro;
namespace tf = tau::fabric;


void try_streams()
{
  cout << "trying streams" << endl;
  tc::null_monitor m;

  tf::pipe<int> s1(8);
  tf::pipe<int> s2(8);

  tc::coro<int> f1("f1", m, [&]() {
    cout << "f1 starting" << endl;
    for (int i = 0;; ++i)
    {
      if (!(s1 << i)) { cout << "s1 rejected " << i << endl; break; }
    }
    s1.close();
    return 0;
  });

  tc::coro<int> f2("f2", m, [&]() {
    cout << "f2 starting" << endl;
    int t = 0;
    int x;
    while (s1 >> x)
    {
      t += x;
      if (!(s2 << t)) { cout << "s2 rejected " << t << endl; break; }
    }
    s1.close();
    s2.close();
    return 0;
  });

  tc::coro<int> f3("f3", m, [&]() {
    cout << "f3 starting" << endl;
    for (int i = 0; i < 1000; ++i)
    {
      int x;
      if (!(s2 >> x)) { cout << "s2 gave no input" << endl; break; }
      cout << x << endl;
    }
    s2.close();
    return 0;
  });

  cout << "s1: " << s1 << endl;
  cout << "s2: " << s2 << endl;

  while (!f1.done() || !f2.done() || !f3.done())
  {
    cout << "s1: " << s1 << endl;
    cout << "s2: " << s2 << endl;
    cout << f1.done() << ", " << f2.done() << ", " << f3.done() << endl;

    if (!f1.done()) f1();
    if (!f2.done()) f2();
    if (!f3.done()) f3();
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
