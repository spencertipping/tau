#ifndef τ²epoll_h
#define τ²epoll_h


#include <errno.h>
#include <fcntl.h>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


#include "../types.hh"
#include "../Lambda.hh"
#include "../gate.hh"

#include "../tau-common.hh"


#include "../begin.hh"

namespace τ
{


struct ψ;


struct τe : public τb
{
  τe(τe&)  = delete;
  τe(τe&&) = delete;
  τe() : τb(), efd(epoll_create1(0)), fin(false)
    { A(efd != -1, "epoll_create1 failure " << errno);
      init_signals(); }

  ~τe()
    { V<fd_t> fds;
      for (let &[fd, g] : gs) fds.push_back(fd);
      for (let x : fds) close(x);
      if (efd != -1) ::close(efd); }


  void init_signals();


  // Closes all managed FDs and the epoll FD, all without modifying the epoll
  // interest list. This is used in τfork to leave the original epoll intact
  // and create a new one for the child process.
  void detach();


  // λg pair: one gate to wait until a file is readable, one for write;
  // gates return false on error.
  struct λgs
  {
    bool re;     // registered for read events
    bool we;     // registered for write events
    λg<bool> r;  // inline read availability, false on error
    λg<bool> w;  // inline write availability, false on error
    λg<bool> e;  // out-of-band error reports
  };


  // Set nonblocking status for a FD, returning the FD. This is done
  // automatically when a FD is added to the epoll_wait set.
  static fd_t nb(fd_t fd)
    { fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
      return fd; }


  λg<bool> *rg(fd_t fd) { if (let g = at(fd)) return &g->r; else return nullptr; }
  λg<bool> *wg(fd_t fd) { if (let g = at(fd)) return &g->w; else return nullptr; }
  λg<bool> *eg(fd_t fd) { if (let g = at(fd)) return &g->e; else return nullptr; }

  iN read  (fd_t fd, u8  *b, uN l)       { return gated(rg(fd), λs::τR, &::read,   fd, b, l); }
  iN write (fd_t fd, u8c *b, uN l)       { return gated(wg(fd), λs::τW, &::write,  fd, b, l); }
  iN pread (fd_t fd, u8  *b, uN l, uN o) { return gated(rg(fd), λs::τR, &::pread,  fd, b, l, o); }
  iN pwrite(fd_t fd, u8c *b, uN l, uN o) { return gated(wg(fd), λs::τW, &::pwrite, fd, b, l, o); }

  fd_t accept(fd_t fd, struct sockaddr *a, socklen_t *l)
    { return gated(rg(fd), λs::τA, &::accept, fd, a, l); }


  // Register a file descriptor for IO events; this must be called before
  // using it with any epoll-mediated syscalls.
  bool reg(fd_t fd, bool r = true, bool w = true);


  // Close an FD and remove it from the epoll watch set. Also delete
  // its gates after awakening both with false to indicate that any
  // pending operations should not move forwards.
  int close(fd_t);


  // Fork and track child PID, return result
  int  fork();
  void term();


  // Call epoll_wait() and invoke all wakeups and Θ-blocked functions.
  // If nonblock = true, epoll_wait() will have a timeout of zero, making
  // it nonblocking.
  τe &operator()(bool nonblock = false);


  operator bool() const
    { return !gs.empty() || l_() || hn() != forever(); }

  τe &go(bool                nonblock = false,
         F<bool(τe&)> const &f        = [](τe &f) { return Sc<bool>(f); });


  // Cancel all ongoing work and unregister all file descriptors, destroying
  // all active ψs. This prepares the τ for new ψs, e.g. after a fork().
  τe &clear();


protected:
  fd_t          efd;   // epoll control FD
  M<fd_t, λgs*> gs;    // edge-triggered gate pairs
  S<pid_t>      pids;  // child pids
  bool          fin;   // true if we're terminating


  // Attempt to allocate an epolled gate pair for the given FD, which
  // is set to nonblocking. Return it if successful, return nullptr
  // if the FD cannot be polled.
  λgs *at(fd_t fd) { return gs.contains(fd) ? gs.at(fd) : nullptr; }


  // Run a function by repeating it against a gate as long as EAGAIN
  // is returned and the associated fd is gated.
  //
  // errno represents the syscall's error state when this function
  // returns.
  template<class F, class... Xs>
  iN gated(λg<bool> *g, λs ys, F *f, Xs... xs)
    { iN r, e;
      while ((r = f(xs...)) == -1 &&
             ((e = errno) == EAGAIN || e == EINTR))
        // NOTE: errno may have been async-reset within g->y, so restore
        // it here before breaking
        if (!g || !g->y(ys)) { errno = e; break; }
      return r; }
};


}

#include "../end.hh"


#endif
