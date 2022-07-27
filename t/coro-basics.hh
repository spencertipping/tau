#include <chrono>
#include <iostream>
#include <queue>

#include "unistd.h"

#include "../tau.hh"


#include "../tau/module/begin.hh"

namespace t::coro_basics
{

using namespace std;
using namespace tau;
using namespace tau::kern;

template<class T>
using co = tau::kern::coro::coro<T>;


template <typename T>
struct stream
{
  deque<T> xs;
  int      capacity;
  bool     omega = false;

  stream(int c) : capacity(c) {}

  inline bool head_ready() const { return !omega && xs.size() < capacity; }
  inline bool tail_ready() const { return !omega && xs.size() > 0; }
  inline T&&  pop()
    {
      while (!tail_ready()) tau::kern::coro::yield();
      T&& x = std::move(xs[0]);
      xs.pop_front();
      return std::move(x);
    }

  inline stream<T> &operator<<(T &x)
    {
      while (!head_ready()) tau::kern::coro::yield();
      xs.push_back(x);
      return *this;
    }
};


void try_streams()
{
  cout << "trying streams" << endl;

  stream<int> s1(4);
  stream<int> s2(4);

  co<int> f1([&]() {
    cout << "f1 started" << endl;
    cout << &s1 << ", " << &s2 << endl;
    for (int i = 0;; ++i)
    {
      cout << "f1 emitting " << i << endl;
      s1 << i;
    }
    return 0;
  });

  co<int> f2([&]() {
    cout << "f2 started" << endl;
    cout << &s1 << ", " << &s2 << endl;
    for (int t = 0;;)
    {
      cout << "f2 requesting" << endl;
      cout << &s1 << endl;
      t += s1.pop();
      cout << "f2 sending " << t << endl;
      s2 << t;
    }
    return 0;
  });

  co<int> f3([&]() {
    cout << "f3 started" << endl;
    for (int i = 0; i < 10; ++i) cout << s2.pop() << endl;
    s2.omega = true;
    return 0;
  });

  while (!s2.omega)
  {
    cout << "s1: " << s1.head_ready() << "/" << s1.tail_ready() << "/" << s1.omega << endl;
    cout << "s2: " << s2.head_ready() << "/" << s2.tail_ready() << "/" << s2.omega << endl;

    if      (!f1.done() && s1.head_ready())                    f1();
    else if (!f2.done() && s1.tail_ready() && s2.head_ready()) f2();
    else if (!f3.done() && s2.tail_ready())                    f3();
  }

  cout << "all done" << endl;
}


void bench()
{
  cout << "bench" << endl;

  stream<int> s1(4);
  stream<int> s2(256);


  co<int> f1([&]() {
    for (int i = 0; i < 1000000; ++i) s1 << i;
    return 1000000;
  });

  {
    let start = stopwatch::now();
    uint64_t t = 0;
    while (!f1.done())
    {
      while (!f1.done() && s1.head_ready()) f1();
      while (s1.tail_ready()) t += s1.pop();
    }
    let end = stopwatch::now();
    cout << "1M via stream(4), yielding " << t << ": " << end - start << endl;
    cout << "f1 returned " << f1.result() << endl;
  }


  co<int> f2([&]() {
    for (int i = 0; i < 1000000; ++i) s2 << i;
    return 1000000;
  });

  {
    let start = stopwatch::now();
    uint64_t t = 0;
    while (!f2.done())
    {
      while (!f2.done() && s2.head_ready()) f2();
      while (s2.tail_ready()) t += s2.pop();
    }
    let end = stopwatch::now();
    cout << "1M via stream(256), yielding " << t << ": " << end - start << endl;
    cout << "f2 returned " << f2.result() << endl;
  }
}


int main()
{
  try_streams();
  bench();
  return 0;
}

}


#include "../tau/module/end.hh"
