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

  cout << "first part is ok" << endl;


  continuation *f2 = new continuation(callcc([&](continuation &&cc) {
    cout << "entered f2" << endl;
    cc = cc.resume();

    cout << "resumed f2 once (ok)" << endl;
    cc = cc.resume();

    cout << "resumed f2 twice (this should not happen)" << endl;
    return std::move(cc);
  }));

  cout << "main: resuming f2 once" << endl;
  *f2 = f2->resume();
  cout << "main: destroying f2" << endl;
  delete f2;
  cout << "main: destroyed f2" << endl;

  cout << "all done" << endl;

  return 0;
}
