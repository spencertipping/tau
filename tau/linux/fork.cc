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
    (γfr(lr) | g | γfw(rw))(τe{}).go();
    //(γfr(lr) | γb(γfw(rw)) | g)(τe{}).go();
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
