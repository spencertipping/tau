#include "../arch.hh"
#if τlinux

#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif

#include <fcntl.h>
#include <unistd.h>

#include "fork.hh"
#include "io.hh"
#include "../share.hh"

#include "../begin.hh"

namespace τ
{


struct γτfork_ : public virtual γ_
{
  γτfork_(γ g_) : g(g_) {}

  St name() const { return "∷[" + g.name() + "]"; }
  void operator()(Ξ&) const;

  γ g;
};


void γτfork_::operator()(Ξ &x) const
{
  let [lr, lw] = pipe_();
  let [rr, rw] = pipe_();
  let child = fork();
  A(child != -1, "fork() failed: " << strerror(errno));

  if (!child)
  {
    close(lw); close(rr);
    x.t().clear();
    (γfr(lr) | γb(γfw(rw)) | g)(x);
    x.t().go();
    exit(0);
  }
  else
  {
    close(lr); close(rw);
    (γfw(lw) | γfr(rr))(x);
  }
}


γ γτfork(γ g) { return new γτfork_{g}; }


}

#include "../end.hh"

#endif
