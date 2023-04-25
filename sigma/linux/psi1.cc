#include "../auto.hh"
#include "../psi1.hh"
#include "../begin.hh"

namespace σ
{

using namespace τ;


φ<Ψ1> φΨ1()
{
  slet p = φd<Ψ1>("Ψ1",
                  "<F", φauto(ΓrF));
  return p;
}


Ψ1 ΓrF(fd_t fd)
{
  return [=](ψ q, ξo o, Ψaux)
    {
      St d; d.reserve(65536);
      τe &t = q.t();
      t.reg(fd, true, false);
      q.fx([&, fd](ψ_&) { shutdown(fd, SHUT_RD); t.close(fd); });
      for (iS r;
           (r = t.read(fd, Rc<u8*>(d.data()), d.capacity())) > 0;)
        o.r(r + 8) << Stv{d.data(), Sc<uN>(r)};
    };
}


}

#include "../end.hh"
