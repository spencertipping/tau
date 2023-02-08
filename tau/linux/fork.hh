#ifndef τlinux_fork_h
#define τlinux_fork_h

#include "../gamma.hh"
#include "../begin.hh"

namespace τ
{


// fork() and exec(), with stdin → γ → stdout. By default stderr is
// inherited from parent, but you can bind it to a Ξ channel by specifying
// a key to bind it to.
Sp<γ> fork_exec(Vc<St>&, St);


}

#include "../end.hh"

#endif
