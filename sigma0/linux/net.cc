#include "../arch.hh"
#if τlinux


#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>


#include "net.hh"
#include "../share.hh"
#include "../begin.hh"

namespace τ
{


Γ tcp_server(u16 port, u32 addr)
{
  return Γffn((Ss{} << "tcp:" << port).str(), [=](Sp<ψ> q, ξi i, ξo o)
    {
      i.close();

      auto &t = q->t();
      fd_t        sfd;
      sockaddr_in sa;

      A((sfd = socket(AF_INET, SOCK_STREAM, 0)) != -1, "socket()");
      q->xf([&, sfd](ψ&) { t.close(sfd); });

      bzero(&sa, sizeof(sa));
      sa.sin_family = AF_INET;
      sa.sin_addr.s_addr = htonl(addr);
      sa.sin_port = htons(port);
      A(!bind(sfd, (sockaddr*)&sa, sizeof(sa)), "bind(tcp:" << port << ")");
      A(!listen(sfd, 16), "listen(tcp:" << port << ")");

      A(t.reg(sfd, true, false), "!τ.reg(" << sfd << ")");

      sockaddr  a;
      socklen_t l;
      for (fd_t c; l = sizeof(a), (c = t.accept(sfd, &a, &l)) != -1;)
        if (!(o << η1o(c))) return;
    });
}


}

#include "../end.hh"

#endif
