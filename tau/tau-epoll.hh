#ifndef τ²epoll_h
#define τ²epoll_h


#include <algorithm>

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


#include "types.hh"
#include "Lambda.hh"
#include "gate.hh"
#include "psi.hh"

#include "tau-common.hh"


#include "begin.hh"

namespace τ
{


struct τe : public τb
{
  τe(τe&)  = delete;
  τe(τe&&) = delete;
  τe() : τb(), efd(epoll_create1(0))
    { // Important: ignore SIGPIPE so we can catch it as an error on FD ops
      signal(SIGPIPE, SIG_IGN);
      A(efd != -1, "epoll_create1 failure " << errno); }

  ~τe()
    { for (let &[fd, g] : gs)
      { g->r.w(false);
        g->w.w(false);
        delete g;
        close(fd); }
      A(!close(efd), "~τ close failed (fd leak) " << errno); }


  // λg pair: one gate to wait until a file is readable, one for write;
  // gates return false on error.
  struct λgs
  {
    λg<bool> r;
    λg<bool> w;
  };


  // Set nonblocking status for a FD, returning the FD. This is done
  // automatically when a FD is added to the epoll_wait set.
  static fd_t nb(fd_t fd)
    { fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
      return fd; }


  λg<bool> *rg(fd_t fd) { if (let g = at(fd)) return &g->r; else return nullptr; }
  λg<bool> *wg(fd_t fd) { if (let g = at(fd)) return &g->w; else return nullptr; }

  iN read  (fd_t fd, u8  *b, uN l)       { return gated(rg(fd), λs::τR, &::read,   fd, b, l); }
  iN write (fd_t fd, u8c *b, uN l)       { return gated(wg(fd), λs::τW, &::write,  fd, b, l); }
  iN pread (fd_t fd, u8  *b, uN l, uN o) { return gated(rg(fd), λs::τR, &::pread,  fd, b, l, o); }
  iN pwrite(fd_t fd, u8c *b, uN l, uN o) { return gated(wg(fd), λs::τW, &::pwrite, fd, b, l, o); }


  // TODO: fork() with a γ argument to create the new pipeline
  // use FIFO + multiplexed communication


  // Close an FD and remove it from the epoll watch set. Also delete
  // its gates after awakening both with false to indicate that any
  // pending operations should not move forwards.
  int close(fd_t fd)
    { if (gs.contains(fd))
      { let g = gs.at(fd);
        g->r.w(false);
        g->w.w(false);
        delete g;
        gs.erase(fd); }
      return ::close(fd); }


  // Call epoll_wait() and invoke all wakeups and Θ-blocked functions.
  // If nonblock = true, epoll_wait() will have a timeout of zero, making
  // it nonblocking.
  τe &operator()(bool nonblock = false)
    { while (now() < hn() && (!gs.empty() || hn() != forever()))
      { epoll_event evs[16];
        let dt = (hn() - now()) / 1ms;
        let n  = epoll_wait(efd, evs, sizeof(evs) / sizeof(epoll_event),
                            nonblock ? 0 : std::min(dt, Sc<decltype(dt)>(Nl<int>::max())));
        A(n != -1, "epoll_wait error " << errno);
        if (!n) break;

        for (int i = 0; i < n; ++i)
        { let f = evs[i].data.fd;
          if (gs.contains(f))
          { let g = at(f);
            if (evs[i].events & EPOLLIN)  g->r.w(true);
            if (evs[i].events & EPOLLOUT) g->w.w(true);
            if (evs[i].events & EPOLLERR) g->w.w(false); }}

        // NOTE: continue to poll if we have more events, but don't block
        // for anything other than the first one -- we need to run another
        // Λ quantum to consume the events.
        nonblock = true; }

      while (now() >= hn()) l.r(h.top().l), h.pop();
      return *this; }


  operator bool() const
    { return !gs.empty() || l || hn() != forever(); }

  τe &go(bool                nonblock = false,
         F<bool(τe&)> const &f        = [](τe &f) { return Sc<bool>(f); })
    { l.go();
      while (f(*this)) (*this)(nonblock), l.go();
      return *this; }


  τe &pin  (Sp<ψ> q) { qs.insert(q); return *this; }
  τe &unpin(Sp<ψ> q) { qs.erase(q);  return *this; }


protected:
  fd_t          efd;  // epoll control FD
  M<fd_t, λgs*> gs;   // edge-triggered gate pairs
  S<Sp<ψ>>      qs;   // boundary-pinned ψs


  // Attempt to allocate an epolled gate pair for the given FD, which
  // is set to nonblocking. Return it if successful, return nullptr
  // if the FD cannot be polled.
  λgs *at(fd_t fd)
    { if (!gs.contains(fd))
      { epoll_event ev;
        ev.events  = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLET;
        ev.data.fd = nb(fd);
        if (epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev) == -1) return nullptr;
        return gs[fd] = new λgs{l, l}; }
      return gs.at(fd); }


  // Run a function by repeating it against a gate as long as EAGAIN
  // is returned and the associated fd is gated.
  //
  // errno represents the syscall's error state when this function
  // returns.
  template<class F, class... Xs>
  iN gated(λg<bool> *g, λs ys, F *f, Xs... xs)
    { iN r, e;
      while ((r = f(xs...)) == -1 && (e = errno) == EAGAIN)
        // NOTE: errno may have been async-reset within g->y, so restore
        // it here before breaking
        if (!g || !g->y(ys)) { errno = e; break; }
      return r; }
};


}

#include "end.hh"


#endif
