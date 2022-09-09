#include <netinet/in.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netdb.h>

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
        ι(n) | g.β();
        g.β().ω();
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
        for (let x : g) g.Θ(dt) << x;
        g.ω();
        return 0;
      });
    }
};


struct splitter
{
  γ  g;
  uN a = 0;
  splitter(Φ &f) : g(f)
    {
      g.λc([&]() {
        ++a;
        for (let x : g)
        {
          std::cout << "splitter < " << x << std::endl;
          if (!(g.δ() << x)) break;
        }
        std::cout << "splitter read loop done" << std::endl;
        if (!--a) g.ω();
        return 0;
      });

      g.λc([&]() {
        ++a;
        for (let x : g.δ())
        {
          std::cout << "splitter > " << x << std::endl;
          if (!(g << x)) break;
        }
        std::cout << "splitter write loop done" << std::endl;
        g.δ().rω();
        if (!--a) g.ω();
        return 0;
      });
    }
};


struct broadcast
{
  γ     g;
  φi    p;
  S<φi> ps;

  broadcast(Φ &f) : g(f)
    {
      p = g.φc(ζb0, true);
      g.λc([&]() {
        for (φi q : g[p])
        {
          std::cout << "broadcast conn; φ = " << q << std::endl;
          g.λc([&, q]() {
            ps.emplace(q);
            for (let x : g[q])
              for (let p2 : ps)
                g[p2] << x;
            ps.erase(q);
            g[q].ω();
            return 0;
          });
        }
        g.ω();
        std::cout << "broadcast done listening" << std::endl;
        return 0;
      });
    }
};


template<class T>
struct entuple
{
  γ g;
  T x;

  entuple(Φ &f, T x)
    {
      g.λc([&]() {
        for (let y : g) g << o9t(x, y);
        g.ω();
        return 0;
      });
    }
};


struct tcp_server
{
  γ  g;
  uN port;

  tcp_server(Φ &f, uN port_) : g(f), port(port_)
    {
      iN sfd;
      sockaddr_in sa;

      A((sfd = socket(AF_INET, SOCK_STREAM, 0)) != -1, "socket()");
      bzero(&sa, sizeof(sa));
      sa.sin_family = AF_INET;
      sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
      sa.sin_port = htons(port);
      A(!bind(sfd, (sockaddr*)&sa, sizeof(sa)), "bind()");
      A(!listen(sfd, 16), "listen()");

      g.λc([&, sfd]() {
        Φf<o9acc> i{g.f, Sc<uN>(sfd)};
        while (g << i);
        g.ω();
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
        Φf<o9fdr> i{g.f, fd, 6};
        while (g.β() << i);
        g.ω();
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
        Φf<o9fdr> o{g.f, fd};
        for (let x : g) if (!(x >> o)) return 1;
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
        for (let x : g)
          //s << "\033[s\033[40G" << x << "\033[r" << std::flush;
          s << std::endl << x << std::endl;
        return 0;
      });
    }
};


#include "../tau/end.hh"
