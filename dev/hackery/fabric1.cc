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
  inline T    pop()
    {
      while (!tail_ready()) boost::this_fiber::yield();
      return xs.pop_front();
    }

  inline stream<T> &operator<<(T &x)
    {
      while (!head_ready()) boost::this_fiber::yield();
      xs.push_back(x);
      return *this;
    }

};





void
generator()
{
  for (int i = 0; ; ++i)
    q1.push_back(i);
  boost::this_fiber::yield();
}


int
main()
{
  return 0;
}
