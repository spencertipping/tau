#include <stdlib.h>
#include <sys/select.h>

#define tau_debug 1
//#define tau_debug_i9st 1
#include "../tau.hh"
#include "../tau/debug.hh"


using namespace tau;
using namespace std;


#include "../tau/begin.hh"


struct fd_in
{
  γ  g;
  Φf i;

  fd_in(Φ &f_, uN fd_) : g(f_), i(f_, fd_)
    {
      g.λc([&]() {
        while (g.ο() << i)
          cout << "fd_in iteration" << endl;
        return 0;
      });
    }
};


struct fd_out
{
  γ  g;
  Φf o;

  fd_out(Φ &f_, uN fd_) : g(f_), o(f_, fd_)
    {
      g.λc([&]() {
        for (let x : g.ι())
          if (!(x >> o))
            return 1;
          else
            cout << "fd_out iteration" << endl;
        return 0;
      });
    }
};


struct stream_out
{
  γ  g;
  O &s;

  stream_out(Φ &f, O &s_ = std::cout) : g(f), s(s_)
    {
      g.λc([&]() {
        for (let x : g.ι()) s << x << std::endl;
        return 0;
      });
    }
};


#include "../tau/end.hh"
