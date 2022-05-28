#include <iostream>
#include <queue>

#include <boost/context/continuation.hpp>


using namespace std;
using namespace boost::context;


static continuation scheduler;


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
      while (!tail_ready()) scheduler = scheduler.resume();
      T&& x = move(xs[0]);
      xs.pop_front();
      return move(x);
    }

  inline stream<T> &operator<<(T &x)
    {
      while (!head_ready()) scheduler = scheduler.resume();
      xs.push_back(x);
      return *this;
    }
};


int
main()
{
  stream<int> s1(4);
  stream<int> s2(4);

  continuation f1(callcc([&](continuation &&cc) {
    cout << "f1 started" << endl;
    scheduler = cc.resume();

    for (int i = 0;; ++i)
    {
      cout << "f1 emitting " << i << endl;
      s1 << i;
    }

    return move(scheduler);
  }));

  continuation f2(callcc([&](continuation &&cc) {
    cout << "f2 started" << endl;
    scheduler = cc.resume();

    for (int t = 0;;)
    {
      cout << "f2 requesting" << endl;
      t += s1.pop();
      cout << "f2 sending " << t << endl;
      s2 << t;
    }

    return move(scheduler);
  }));

  continuation f3(callcc([&](continuation &&cc) {
    cout << "f3 started" << endl;
    scheduler = cc.resume();

    for (int i = 0; i < 10; ++i) cout << s2.pop() << endl;
    s2.omega = true;

    return move(scheduler);
  }));

  while (!s2.omega)
  {
    cout << "s1: " << s1.head_ready() << "/" << s1.tail_ready() << "/" << s1.omega << endl;
    cout << "s2: " << s2.head_ready() << "/" << s2.tail_ready() << "/" << s2.omega << endl;

    if (s1.head_ready())                    f1 = f1.resume();
    if (s1.tail_ready() && s2.head_ready()) f2 = f2.resume();
    if (s2.tail_ready())                    f3 = f3.resume();
  }

  cout << "all done" << endl;

  return 0;
}
