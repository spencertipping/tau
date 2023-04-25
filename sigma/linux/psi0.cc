#include "../auto.hh"
#include "../psi0.hh"
#include "../begin.hh"

namespace σ
{

using namespace τ;


Ψ0 ΓwF(fd_t fd)
{
  return [=](ψ q, ξi i, Ψaux)
    {
      τe &t = q.t();
      q.pin();
      q.fx([&, fd](ψ_&) { shutdown(fd, SHUT_WR); t.close(fd); });
      t.reg(fd, false, true);
      for (let x : i)
        if (x.is_ω()) goto done;
        else if (x.is_s())
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
    };
}


}

#include "../end.hh"
