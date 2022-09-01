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
  γ   g;
  uNc fd;

  fd_in(Φ &f_, uN fd_) : g(f_), fd(fd_)
    {
      g.λc([&]() {
        Φf i{g.f, fd, 6};
        auto &o = g.ο();
        while (o << i)
          g.Θ(10ms);
        o.ω();
        return i.rx();
      });
    }
};


struct fd_out
{
  γ   g;
  uNc fd;

  fd_out(Φ &f_, uN fd_) : g(f_), fd(fd_)
    {
      g.λc([&]() {
        Φf o{g.f, fd};
        auto &i = g.ι();
        while (i.ri())
        {
          auto x = *i;
          if (!(x >> o))
          {
            cerr << "fd_out write failed, returning 1" << endl;
            return 1;
          }
          x.free();
          ++i;
        }
        cerr << "end of input, fd_out returning 0" << endl;
        i.ω();
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
