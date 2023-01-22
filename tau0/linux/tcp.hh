#ifndef τξlinux_tcp_h
#define τξlinux_tcp_h


#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>


#include "../begin.hh"


namespace τ
{


struct o9acc  // accept socket connection from server FD
{
  iN         &n;
  iN         &e;
  fd_t const  fd;

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
      auto t = o9t(u9_scoped<u9_Φ, fd_t>{u9_Φ::fd, Sc<fd_t>(n)},
                   Bv{Rc<u8*>(&sa), l});
      t.write(m);
      return t.size(); }
};

template<> struct o9_<o9acc> { sletc v = true; };


template<>
inline void Φf<o9acc>::init()
{
  Φnb(o.fd);
  ep = f << *this ? 0 : errno;
}

template<>
inline Φf<o9acc>::~Φf()
{
  f.x(*this);
}


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

  return *new ϝ(f, "tcp_server", ϝ::ξΦ, [&, sfd](ϝ &f, γ &g)
    { Φf<o9acc> i{g.f, Sc<fd_t>(sfd)};
      while (f << i); });
}


}
}


#include "../end.hh"


#endif
