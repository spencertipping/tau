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


struct Γτfork_ : public virtual Γ_
{
  Γτfork_(Γ g_) : g(g_) {}

  St name() const { return "∷[" + g.name() + "]"; }
  void operator()(Ξ&) const;

  Γ g;
};


void Γτfork_::operator()(Ξ &x) const
{
  let [lr, lw] = pipe_();
  let [rr, rw] = pipe_();
  let child = x.t().fork();
  A(child != -1, "fork() failed: " << strerror(errno));

  if (!child)
  {
    // Important: x.t() holds the same epoll() as the original τ, so we
    // can't modify it by e.g. closing fds that it owns (since that will
    // remove them from the epoll interest list). Instead, we need to
    // create a new epoll() for the child process.
    //
    // The final step here is to close out the original epoll _without_
    // modifying the interest list, since it may be destroyed soon.
    x.t().detach();
    close(lw); close(rr);
    (Γfr(lr) | Γfw(rw, true) | g)(τe{}).go();
    exit(0);
  }
  else
  {
    close(lr); close(rw);
    (Γfw(lw) | Γfr(rr))(x);
  }
}


Γ Γτfork(Γ g) { return new Γτfork_{g}; }


}

#include "../end.hh"

#endif
