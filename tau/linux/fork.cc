#include "../arch.hh"
#if τplatform == τplatform_linux

#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif

#include <fcntl.h>
#include <unistd.h>

#include "fork.hh"
#include "io.hh"
#include "../Gamma.hh"
#include "../begin.hh"

namespace τ
{


struct γτfork_ : public virtual γ
{
  γτfork_(Sp<γ> g_) : g(g_) {}

  St name() const { return "∷[" + g->name() + "]"; }
  Ξ &operator()(Ξ&);

  Sp<γ> g;
};


Ξ &γτfork_::operator()(Ξ &x)
{
  let [lr, lw] = pipe_();
  let [rr, rw] = pipe_();
  let child = fork();
  A(child != -1, "fork() failed: " << strerror(errno));

  if (!child)
  {
    close(lw); close(rr);
    x.t().clear();
    (*g)(x);
    x.t().go();
    exit(0);
  }
  else
  {
    close(lr); close(rw);
    (γfw(lw) | γfr(rr))(x);
  }

  return x;
}


Sp<γ> γτfork(Sp<γ> g)
{ return Sp<γ>{new γτfork_{g}}; }


}

#include "../end.hh"

#endif
