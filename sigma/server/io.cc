#include <netdb.h>
#include <sys/socket.h>

#include "io.hh"
#include "../begin.hh"

namespace σ
{

using namespace τ;


// FIXME: reader + writer for sockets; otherwise we close the FD
// on the read-side before the write-side is done (sometimes)


slet ΓrF_ = Ψauto([](fd_t fd, ψ q, ξo o)
  {
    St d; d.reserve(65536);
    τe &t = q.t();
    q.fx([&, fd](ψ_&) { shutdown(fd, SHUT_RD); t.close(fd); });

    if (t.detached()) return;
    t.reg(fd, true, false);

    for (iS r;
         (r = t.read(fd, Rc<u8*>(d.data()), d.capacity())) > 0;)
      o.r(r + 8) << Stv{d.data(), uS(r)};
  });


slet ΓwF_ = Ψauto([](fd_t fd, ψ q, ξi i)
  {
    τe &t = q.t();
    q.pin();
    q.fx([&, fd](ψ_&) { shutdown(fd, SHUT_WR); t.close(fd); });
    if (t.detached()) goto done;
    t.reg(fd, false, true);
    for (let &x : i)
      if (x.is_ω()) goto done;
      else if (x.is_s() || x.is_bin())
      {
        uN w = 0;
        while (w < x.size())
        {
          let n = t.write(fd, x.data() + w, x.size() - w);
          if (n < 0) goto done;
          else       w += n;
        }
      }
  done:
    i.close();
    q.unpin();
  });


slet ΓT_ = Ψauto([](i64 addr, φaL<':'>, i64 port, φig, Γa<Γ> g, ψ q, ξo o)
  {
    τe &t = q.t();
    fd_t fd = socket(AF_INET, SOCK_STREAM, 0);
    q.fx([&, fd](ψ_&) { t.close(fd); });
    if (t.detached()) return;
    A(t.reg(fd, true, false), "τ::reg(<" << fd << ") failed");

    sockaddr_in a;
    a.sin_family = AF_INET;
    a.sin_port = htons(port);
    a.sin_addr.s_addr = htonl(addr);
    A(!bind(fd, (sockaddr*)&a, sizeof(a)),
      "bind to port " << port << ", addr " << addr << " failed");

    // We may not get back to this socket for a while, so we want to
    // have enough space to hold many inbound connections.
    listen(fd, 64);

    for (;;)
    {
      let c = t.accept(fd, nullptr, nullptr);
      o.r() << c;
      (ΓrF(c) | ΓwF(c, Ψd::b) | g.x)(Ξ{t}.push());
    }
  });


Γ ΓrF(fd_t fd, Ψd d) { return ΓΨ(ΓrF_(fd), d, (Ss{} << "<F" << fd).str()); }
Γ ΓwF(fd_t fd, Ψd d) { return ΓΨ(ΓwF_(fd), d, (Ss{} << ">F" << fd).str()); }

Γ ΓT(i64 a, i64 p, Γ g, Ψd d)
{
  return ΓΨ(ΓT_(a, {}, p, {}, {g}), d,
            (Ss{} << "T" << a << ":" << p << " " << g).str());
}


void Γio(Γφ &g)
{
  g.def_p0(">F", ΓwF_);
  g.def_p1("<F", ΓrF_);
  g.def_p1("T",  ΓT_);
}


}

#include "../end.hh"
