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

  fd_in(Φ &f_, uN fd) : g(f_)
    {
      g.λc([&]() {
        Φf i{g.f, fd};
        cerr << "starting input loop" << endl;
        while (g.ο() << i)
          cerr << "fd_in iteration" << endl;

        cerr << "return case" << endl;
        cerr << "  i.o.n = " << i.o.n << endl;
        cerr << "  i.o.e = " << i.o.e << endl;
        cerr << "  i.rx() = " << i.rx() << endl;

        cerr << "fd_in output was rejected, returning" << endl;
        g.ο().ω();

        return 0;
      });
    }
};


struct fd_out
{
  γ  g;

  fd_out(Φ &f_, uN fd) : g(f_)
    {
      g.λc([&]() {
        Φf o{g.f, fd};
        for (let x : g.ι())
          if (!(x >> o))
          {
            cerr << "write failed, early return" << endl;
            return 1;
          }
          else
            cerr << "fd_out iteration" << endl;
        cerr << "fd_out returning; end of input" << endl;
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
