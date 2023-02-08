#ifndef τlinux_fork_h
#define τlinux_fork_h

#include "../gamma.hh"
#include "../begin.hh"

namespace τ
{


inline P<fd_t, fd_t> pipe_()
{
  fd_t fds[2];
  A(!pipe(fds), "pipe() failed: " << strerror(errno));
  return mp(fds[0], fds[1]);
}


// fork() and exec(), with stdin → γ → stdout. By default stderr is
// inherited from parent, but you can bind it to a Ξ channel by specifying
// a key to bind it to.
Sp<γ> γfork_exec(Vc<St>&, St);


// fork() into a new τ running the specified γ, which will be connected
// to the current Ξ via full-duplex in/out (i.e. it's a series connection)
//
// The inner γ is expected to connect as a right loop, i.e. it should be
// an end cap.
Sp<γ> γτfork(Sp<γ>);


}

#include "../end.hh"

#endif
