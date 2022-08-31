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
  γ    g;
  uN   fd;
  bool go = true;

  fd_in(Λ &l, uN fd_) : g(l), fd(fd_)
    {
      g.λc([&]() {
        fd_set rfds;
        timeval tv;
        iN n = 0;
        iN e = 0;

        while (go)
        {
          while (go)
          {
            FD_ZERO(&rfds);
            FD_SET(fd, &rfds);
            tv.tv_sec  = 1;
            tv.tv_usec = 0;

            g.λy();
            if (select(1, &rfds, nullptr, nullptr, &tv))
              break;
          }

          if (go)
          {
            if (!(g.ο() << o9fdr{fd, n, e}))
            {
              g.δ() <<= o9t("write failed, errno", e); g.λy();
              g.ο().ω();
              g.δ().ω();
              return 1;
            }
            g.δ() <<= o9t("wrote a value of ", n, " bytes");
            g.λy();
          }
        }

        return 0;
      });

      if (0)
        g.λc([&]() {
          for (let x : g.ξ()) go = Sc<bool>(x);
          return 0;
        });
    }
};


struct fd_out
{
  γ  g;
  uN fd;

  fd_out(Λ &l, uN fd_) : g(l), fd(fd_)
    {
      g.λc([&]() {
        for (let x : g.ι()) write(fd, x.begin(), x.size());
        return 0;
      });
    }
};


struct stream_out
{
  γ  g;
  O &s;

  stream_out(Λ &l, O &s_ = std::cout) : g(l), s(s_)
    {
      g.λc([&]() {
        for (let x : g.ι()) s << x << std::endl;
        return 0;
      });
    }
};


#include "../tau/end.hh"
