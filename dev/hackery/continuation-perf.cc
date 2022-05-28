#include <boost/context/continuation.hpp>
#include <iostream>

using namespace std;
using namespace boost::context;


int main()
{
  long long x    = 0;
  bool      done = false;

  continuation gen(callcc([&](continuation &&cc) {
    for (long long i = 0; i < 1000000000; ++i)
    {
      x = i;
      cc = cc.resume();
    }
    done = true;
    return move(cc);
  }));

  long long t = 0;
  while (!done)
  {
    gen = gen.resume();
    t += x;
  }

  cout << t << endl;

  return 0;
}
