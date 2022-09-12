#ifndef τξtcp_h
#define τξtcp_h


#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>


#include "begin.hh"


namespace τ
{


struct o9acc  // accept socket connection from server FD
{
  uNc  fd;
  iN  &n;
  iN  &e;

  o9acc(uN fd_, iN &n_, iN &e_) : fd(fd_), n(n_), e(e_) {}

  uN size() const { return sizeof(sockaddr) + 256; }
  uN write(ζp m) const
    { sockaddr  sa;
      socklen_t l = 0;
      bzero(&sa, sizeof(sa));
      if ((n = accept(fd, &sa, &l)) == -1) { e = errno; return ζω; }
      e = 0;

      // important: o9t.write() returns 0, because it fills its size;
      // we, however, requested a different size because we don't know
      // how big l will be. So we need to calculate the return ourselves.
      auto t = o9t(u9_pidfd{Sc<u32>(getpid()), Sc<u32>(n)},
                   Bv{Rc<u8*>(&sa), l});
      t.write(m);
      return t.size(); }
};

template<> struct o9_<o9acc> { sletc v = true; };


template<>
inline Φf<o9acc>::Φf(Φ &f_, uN fd_, u32 s) : f(f_), w{f.l}, o{fd_, rn, re}
{ Φnb(fd_); ep = f << *this ? 0 : errno; }


namespace ξ
{


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


}
}


#include "end.hh"


#endif
