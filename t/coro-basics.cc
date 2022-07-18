#include <chrono>
#include <iostream>
#include <queue>

#include "unistd.h"

#include "../c/coro.hh"
#include "../c/begin.hh"


using namespace std;
using namespace tau::coro;


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
      while (!tail_ready()) yield();
      T&& x = std::move(xs[0]);
      xs.pop_front();
      return std::move(x);
    }

  inline stream<T> &operator<<(T &x)
    {
      while (!head_ready()) yield();
      xs.push_back(x);
      return *this;
    }
};


void try_streams()
{
  cout << "trying streams" << endl;

  stream<int> s1(4);
  stream<int> s2(4);

  coro<int> f1("f1", [&]() {
    cout << "f1 started" << endl;
    cout << &s1 << ", " << &s2 << endl;
    for (int i = 0;; ++i)
    {
      cout << "f1 emitting " << i << endl;
      s1 << i;
    }
    return 0;
  });

  coro<int> f2("f2", [&]() {
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

  coro<int> f3("f3", [&]() {
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


  coro<int> f1("f1", [&]() {
    for (int i = 0; i < 1000000; ++i) s1 << i;
    return 1000000;
  });

  {
    auto start = chrono::steady_clock::now();
    uint64_t t = 0;
    while (!f1.done())
    {
      while (!f1.done() && s1.head_ready()) f1();
      while (s1.tail_ready()) t += s1.pop();
    }
    auto end   = chrono::steady_clock::now();
    chrono::duration<double> d = end - start;
    cout << "1M via stream(4), yielding " << t << ": " << d.count() << "s" << endl;
    cout << "f1 returned " << f1.result() << endl;
  }


  coro<int> f2("f2", [&]() {
    for (int i = 0; i < 1000000; ++i) s2 << i;
    return 1000000;
  });

  {
    auto start = chrono::steady_clock::now();
    uint64_t t = 0;
    while (!f2.done())
    {
      while (!f2.done() && s2.head_ready()) f2();
      while (s2.tail_ready()) t += s2.pop();
    }
    auto end   = chrono::steady_clock::now();
    chrono::duration<double> d = end - start;
    cout << "1M via stream(256), yielding " << t << ": " << d.count() << "s" << endl;
    cout << "f2 returned " << f2.result() << endl;
  }
}


int main()
{
  coro_init();
  try_streams();
  bench();

  return 0;
}


#include "../c/end.hh"
