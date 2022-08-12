#include <iostream>
#include <queue>

#include <boost/context/continuation.hpp>


using namespace std;
using namespace boost::context;


static continuation scheduler;


int
main()
{
  continuation f1(callcc([&](continuation &&cc) {
    cout << "f1 started" << endl;
    cc = cc.resume();

    continuation fn(callcc([&](continuation &&cc_) {
      cout << "nested continuation started" << endl;
      cc_ = cc_.resume();
      cout << "nested continuation re-entered" << endl;
      return std::move(cc_);
    }));

    cout << "f1 re-entered" << endl;
    fn = fn.resume();
    cout << "f1 is done" << endl;
    return std::move(cc);
  }));

  f1.resume();

  cout << "all done" << endl;

  return 0;
}
