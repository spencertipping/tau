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


struct γfork_exec_ : public virtual γ_
{
  γfork_exec_(Vc<St> &argv_, St stderr_) : argv(argv_), stderr(stderr_) {}

  St name() const { return "e[" + argv[0] + "]"; }
  void operator()(Ξ&) const;

  Vc<St> argv;
  St     stderr;
};


void γfork_exec_::operator()(Ξ &x) const
{
  let [lr, lw] = pipe_();
  let [rr, rw] = pipe_();
  let [er, ew] = pipe_();
  let child = fork();
  A(child != -1, "fork() failed: " << strerror(errno));

  if (!child)
  {
    close(lw); close(rr); close(er);
    close(0);  close(1);  close(2);
    dup2(lr, 0); close(lr);
    dup2(rw, 1); close(rw);
    dup2(ew, 2); close(ew);

    fcntl(0, F_SETFL, fcntl(0, F_GETFL) & ~O_CLOEXEC);
    fcntl(1, F_SETFL, fcntl(0, F_GETFL) & ~O_CLOEXEC);
    fcntl(2, F_SETFL, fcntl(0, F_GETFL) & ~O_CLOEXEC);

    ch *argv_[argv.size() + 1];
    argv_[0] = Cc<ch*>(argv[0].c_str());
    for (uN i = 1; i < argv.size(); ++i) argv_[i] = Cc<ch*>(argv[i].c_str());
    argv_[argv.size()] = nullptr;

    A(!execv(argv[0].c_str(), argv_), "execv() failed: " << strerror(errno));
    exit(1);  // important!
  }
  else
  {
    close(lr); close(rw); close(ew);
    (γfw(lw) | γfr(rr)
     | γswap(stderr) | γfr(er) | γswap(stderr))(x);
  }
}


γ γfork_exec(Vc<St> &argv, St stderr)
{ return new γfork_exec_(argv, stderr); }


}

#include "../end.hh"

#endif
