#include <netdb.h>
#include <sys/socket.h>

#include "../auto.hh"
#include "../psi0.hh"
#include "../psi1.hh"
#include "../begin.hh"

namespace σ
{

using namespace τ;


Ψ1 Γtcp(i16 port, i32 addr, Γ g)
{
  return {[=](ψ q, ξo o, Ψaux)
    {
      τe &t = q.t();
      fd_t fd = socket(AF_INET, SOCK_STREAM, 0);
      t.reg(fd, true, false);
      q.fx([&, fd](ψ_&) { t.close(fd); });

      sockaddr_in a;
      a.sin_family = AF_INET;
      a.sin_port = htons(port);
      a.sin_addr.s_addr = htonl(addr);
      A(!bind(fd, (sockaddr*)&a, sizeof(a)),
        "bind to port " << port << ", addr " << addr << " failed");
      listen(fd, 5);

      for (;;)
      {
        let c = t.accept(fd, nullptr, nullptr);
        o.r() << c;
        (ΓΨ(ΓrF(c)) | ΓΨ(ΓwF(c), Ψd::b) | g)(Ξ{t}.push());
      }
    }};
}


Ψ1 ΓrF(fd_t fd)
{
  return {[=](ψ q, ξo o, Ψaux)
    {
      St d; d.reserve(65536);
      τe &t = q.t();
      t.reg(fd, true, false);
      q.fx([&, fd](ψ_&) { shutdown(fd, SHUT_RD); t.close(fd); });
      for (iS r;
           (r = t.read(fd, Rc<u8*>(d.data()), d.capacity())) > 0;)
        o.r(r + 8) << Stv{d.data(), Sc<uN>(r)};
    }};
}


}

#include "../end.hh"
