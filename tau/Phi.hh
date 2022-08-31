#ifndef tau_Φ_h
#define tau_Φ_h


#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>


#include "types.hh"
#include "Lambda.hh"


#include "begin.hh"

namespace tau
{


sletc Φen = 64;     // number of events per epoll_wait call


// TODO: this object model is totally wrong; we need persistent
// file-manipulation objects that un-epoll themselves on destruct


struct Φ
{
  Λ           &l;
  iNc          fd;  // epoll control FD
  M<λi, uN>    rl;  // which FD each λ is waiting for
  M<λi, uN>    wl;  // which FD each λ is waiting for
  M<uN, λi>    le;  // which λ is awaiting each FD
  epoll_event  ev[Φen];


  Φ(Λ &l_) : l(l_), fd(epoll_create1(0))
    { A(fd != -1, "epoll_create1 failure " << errno); }

  ~Φ() { A(close(fd), "~Φ close failed (fd leak)"); }


  iN x(uN f)
    { if (le.contains(f)) { rl.erase(le[f]); wl.erase(le[f]); le.erase(f); }
      return close(f); }


  iN r(uN f, void *b, uN s)
    { let ri = l.i();
      if (!rl.contains(ri))
      { epoll_event ev_;
        ev_.events   = EPOLLIN;
        ev_.data.u64 = ri;
        A(epoll_ctl(fd, EPOLL_CTL_ADD, f, &ev_) != -1,
          "epoll_ctl add failed for " << f << ": " << errno);
        le[rl[ri] = f] = ri; }
      l.y(λs::ΦI);
      return read(f, b, s); }


  iN w(uN f, void *b, uN s)
    { let ri = l.i();
      if (!wl.contains(ri))
      { epoll_event ev_;
        ev_.events   = EPOLLOUT;
        ev_.data.u64 = ri;
        A(epoll_ctl(fd, EPOLL_CTL_ADD, f, &ev_) != -1,
          "epoll_ctl add failed for " << f << ": " << errno);
        le[wl[ri] = f] = ri; }
      l.y(λs::ΦO);
      return write(f, b, s); }


};


}

#include "end.hh"


#endif
