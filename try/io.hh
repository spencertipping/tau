#include <iostream>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netdb.h>

#define tau_debug 1
//#define tau_debug_i9st 1

#include "../tau.hh"
#include "../tau/debug.hh"


using namespace τ;


#include "../tau/begin.hh"


template<class I = i9, class O>
ϝ &io(Φ &f, F<O(I)> &&l)
{
  return *new ϝ(f, ϝ::ξι, [&, l = std::move(l)](ϝ &f)
    { for (let x : f) if (!(f << l(x))) break; });
}


ϝ &iota(Φ &f, uN n = -1)
{
  return *new ϝ(f, ϝ::ξι, [&, n](ϝ &f) { if (!(f < ι(n))) std::cout << "early exit" << std::endl; });
}


ϝ &delay(Φ &f, ΔΘ dt)
{
  return *new ϝ(f, ϝ::ξι, [&, dt](ϝ &f)
    { for (let x : f) if (!(f.Θ(dt) << x)) break; });
}


ϝ &splitter(Φ &f)
{
  return *new ϝ(f, ϝ::ξτ,
                [&](ϝ &f) { f.α() > f.δ(); },
                [&](ϝ &f) { f.ε() > f.α(); });
}


ϝ &stream_out(Φ &f, O &s)
{
  return *new ϝ(f, ϝ::ξΦ, [&](ϝ &f)
    { for (let x : f) s << f.g.f.dt() << ": " << x << std::endl; });
}


ϝ &broadcast(Φ &f)
{
  return *new ϝ(f, ϝ::ξγ, [&](ϝ &f)
    { S<φi> ps;
      for (let c : f)
      { let i = Sc<φi>(c);  // important: i9s are mutable
        f.ε() <<= o9t(i, true);
        f.λc([&, i](ϝ &f) {
          ps.emplace(i);
          for (let x : f[i]) for (let q : ps) f[q] <<= x;
          ps.erase(i);
          f.ε() <<= o9t(i, false);
        }); }});
}


ϝ &tcp_server(Φ &f, uN port, u32 listen_addr = INADDR_LOOPBACK)
{
  iN          sfd;
  sockaddr_in sa;

  A((sfd = socket(AF_INET, SOCK_STREAM, 0)) != -1, "socket()");
  bzero(&sa, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = htonl(listen_addr);
  sa.sin_port = htons(port);
  A(!bind(sfd, (sockaddr*)&sa, sizeof(sa)), "bind()");
  A(!listen(sfd, 16), "listen()");

  return *new ϝ(f, ϝ::ξΦ, [&, sfd](ϝ &f, γ &g)
    { Φf<o9acc> i{g.f, Sc<uN>(sfd)};
      while (f << i); });
}


ϝ &fd_in(Φ &f, uN fd)
{
  return *new ϝ(f, ϝ::ξΦ, [&, fd](ϝ &f, γ &g)
    { Φf<o9fdr> i{g.f, fd}; while (f << i); });
}


ϝ &fd_out(Φ &f, uN fd)
{
  return *new ϝ(f, ϝ::ξΦ, [&, fd](ϝ &f, γ &g)
    { Φf<o9fdr> o{g.f, fd};
      for (let x : f) if (!(x >> o)) break; });
}


ϝ &fd_io(Φ &f, uN fd)
{
  return *new ϝ(f, ϝ::ξΦ,
                [&, fd](ϝ &f, γ &g) { Φf<o9fdr> o{g.f, fd}; for (let x : f) if (!(x >> o)) break; },
                [&, fd](ϝ &f, γ &g) { Φf<o9fdr> i{g.f, fd}; while (f << i); });
}


#include "../tau/end.hh"
