#include <stdlib.h>
#include <sys/select.h>

#define tau_debug 1
//#define tau_debug_i9st 1
#include "../tau.hh"
#include "../tau/debug.hh"


using namespace tau;


#include "../tau/begin.hh"


struct iota
{
  γ  g;
  uN n;

  iota(Φ &f, uN n_ = -1) : g(f), n(n_)
    {
      g.λc([&]() {
        for (uN i = 0; i < n; ++i)
          g.ο() << i;
        g.ο().ω();
        return 0;
      });
    }
};


struct delay
{
  γ  g;
  ΔΘ dt;

  delay(Φ &f, ΔΘ dt_) : g(f), dt(dt_)
    {
      g.λc([&]() {
        for (let x : g.ι()) g.Θ(dt).ο() << x;
        g.ο().ω();
        return 0;
      });
    }
};


struct fd_in
{
  γ   g;
  uNc fd;

  fd_in(Φ &f_, uN fd_) : g(f_), fd(fd_)
    {
      g.λc([&]() {
        Φf i{g.f, fd, 6};
        while (g.ο() << i);
        g.ο().ω();
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
        for (let x : g.ι()) if (!(x >> o)) return 1;
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
        for (let x : g.ι())
          //s << "\033[s\033[40G" << x << "\033[r" << std::flush;
          s << std::endl << x << std::endl;
        return 0;
      });
    }
};


#include "../tau/end.hh"
