#include <iostream>
#include <queue>

#include <boost/fiber/fiber.hpp>
#include <boost/fiber/operations.hpp>


using namespace std;
using namespace boost::fibers;


template <typename T>
struct stream
{
  deque<T> xs;
  int      capacity;

  stream(int c) : capacity(c) {}

  inline bool head_ready() const { return xs.size() < capacity; }
  inline bool tail_ready() const { return xs.size() > 0; }
  inline T&&  pop()
    {
      while (!tail_ready()) boost::this_fiber::yield();
      T&& x = move(xs[0]);
      xs.pop_front();
      return move(x);
    }

  inline stream<T> &operator<<(T &x)
    {
      while (!head_ready()) boost::this_fiber::yield();
      xs.push_back(x);
      return *this;
    }
};


int
main()
{
  stream<int> s1(4);
  stream<int> s2(4);

  fiber f1([&]() { for (int i = 0; ; ++i) s1 << i; });
  fiber f2([&]() { for (int t = 0;;) s2 << (t += s1.pop()); });
  fiber f3([&]() { for (int i = 0; i < 10; ++i) cout << s2.pop() << endl; });

  cout << "fibers are started" << endl;

  f3.join(); cout << "f3 is joined" << endl;
  f2.join(); cout << "f2 is joined" << endl;
  f1.join(); cout << "f1 is joined" << endl;

  return 0;
}
