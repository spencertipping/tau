#ifndef τ²epoll_h
#define τ²epoll_h


#include <errno.h>
#include <fcntl.h>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


#include "../types.hh"
#include "../Lambda.hh"
#include "../gate.hh"

#include "threadpool.hh"

#include "../tau-common.hh"


#include "../begin.hh"

namespace τ
{


struct ψ;

Tt struct λfg;


struct τe : public τb
{
  τe(τe&)  = delete;
  τe(τe&&) = delete;
  τe(int threads = Th::hardware_concurrency())
    : τb(), fin(false), tp(threads), tid(0)
    { A((efd = epoll_create1(0)) != -1, "epoll_create1 failure (τe::efd)");
      fcntl(efd, F_SETFL, fcntl(efd, F_GETFL) | O_CLOEXEC);
      A((wfd = eventfd(0, EFD_SEMAPHORE | EFD_CLOEXEC)) != -1,
        "eventfd() failure (τe::wfd)");

      // IMPORTANT: wfd must be registered _without_ edge triggering or we will
      // get thread deadlocks.
      A(reg(wfd, true, false, false), "wfd reg() failure");
      init_signals(); }

  ~τe()
    { V<fd_t> fds;
      for (let &[fd, g] : rgs) fds.push_back(fd);
      for (let &[fd, g] : wgs) fds.push_back(fd);
      for (let x : fds) close(x);
      if (wfd != -1)   close(wfd);
      if (efd != -1) ::close(efd); }


  void init_signals();


  // Directional λg container: gates for IO and error events
  struct λgs
  {
    λg<bool> io;  // IO is ready, or immediate error
    λg<bool> e;   // out-of-band error
  };


  // Set nonblocking status for a FD, returning the FD. This is done
  // automatically when a FD is added to the epoll_wait set.
  static fd_t nb(fd_t fd)
    { fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK | O_CLOEXEC);
      return fd; }


  λg<bool> *rg(fd_t fd) { if (let g = rat(fd)) return &g->io; else return nullptr; }
  λg<bool> *wg(fd_t fd) { if (let g = wat(fd)) return &g->io; else return nullptr; }

  iN read  (fd_t fd, u8  *b, uN l)       { return gated(rg(fd), λs::τR, ::read,   fd, b, l); }
  iN write (fd_t fd, u8c *b, uN l)       { return gated(wg(fd), λs::τW, ::write,  fd, b, l); }
  iN pread (fd_t fd, u8  *b, uN l, uN o) { return gated(rg(fd), λs::τR, ::pread,  fd, b, l, o); }
  iN pwrite(fd_t fd, u8c *b, uN l, uN o) { return gated(wg(fd), λs::τW, ::pwrite, fd, b, l, o); }

  fd_t accept(fd_t fd, struct sockaddr *a, socklen_t *l)
    { return gated(rg(fd), λs::τA, &::accept, fd, a, l); }


  // Register a file descriptor for IO events; this must be called before
  // using it with any epoll-mediated syscalls.
  bool reg  (fd_t fd, bool r = true, bool w = true, bool et = true);
  void unreg(fd_t fd, bool r = true, bool w = true);


  // Close an FD and remove it from the epoll watch set. Also delete
  // its gates after awakening both with false to indicate that any
  // pending operations should not move forwards.
  int close(fd_t, bool r = true, bool w = true);

  bool detached() const { return efd == -1; }


  // Break out of the epoll_wait loop so we can continue scheduling λs.
  // This is called from background threads.
  uN   new_tid();
  void wake    (uN tid);
  bool is_awake(uN tid);


  // Run a function in a background thread, returning the result once it's done.
  // tp() automatically calls wake for us via its ef() callback.
  Txxs auto operator()(X &&f, Xs&&... xs) -> decltype(f(xs...))
    { return fg(bg(std::forward<X>(f), std::forward<Xs>(xs)...)); }

  Txxs auto bg(X &&f, Xs&&... xs) -> P<uN, Fu<decltype(f(xs...))>>
    { let args = std::make_tuple(std::forward<Xs>(xs)...);
      let id   = new_tid();  // create running task reservation
      return {id, tp([this, f=std::forward<X>(f), xs=mo(args), id]()
        { let r = std::apply(f, xs);
          wake(id);  // remove from trs and wake eventfd
          return r; })}; }

  Tt T fg(P<uN, Fu<T>> &&f)
    { auto &[id, fu] = f;
      while (!is_awake(id)) rg(wfd)->y(λs::T);  // consume eventfd once awake
      return fu.get(); }


  // Handle signals
  void sig(int s);

  void sig_register  (Sp<F<void(int)>> f) { sfs.insert(f); }
  void sig_unregister(Sp<F<void(int)>> f) { sfs.erase(f); }


  // Call epoll_wait() and invoke all wakeups and Θ-blocked functions.
  // If nonblock = true, epoll_wait() will have a timeout of zero, making
  // it nonblocking.
  τe &operator()(bool nonblock = false);


  explicit operator bool() const
    { return rgs.size() > 1 || !wgs.empty()
          || tp.tasks() || l_() || hn() != forever(); }

  τe &go(bool                nonblock = false,
         F<bool(τe&)> const &f        = [](τe &f) { return Sc<bool>(f); });


protected:
  fd_t                efd;    // epoll control FD
  fd_t                wfd;    // eventfd wake FD
  M<fd_t, Sp<λgs>>    rgs;    // read gate sets
  M<fd_t, Sp<λgs>>    wgs;    // write gate sets
  S<pid_t>            pids;   // child pids
  bool                fin;    // true if we're terminating
  thread_pool         tp;
  At<uN>              tid;    // task ID counter
  S<uN>               trs;    // set of running tasks
  Smu                 trs_m;  // shared mutex for trs
  S<Sp<F<void(int)>>> sfs;  // signal handler functions


  // Return the gate set for a FD, or nullptr if it's not registered.
  Sp<λgs> rat(fd_t fd) { return rgs.contains(fd) ? rgs.at(fd) : nullptr; }
  Sp<λgs> wat(fd_t fd) { return wgs.contains(fd) ? wgs.at(fd) : nullptr; }


  // Run a function by repeating it against a gate as long as EAGAIN
  // is returned and the associated fd is gated.
  //
  // errno represents the syscall's error state when this function
  // returns.
  template<class F, class... Xs>
  iN gated(λg<bool> *g, λs ys, F const &f, Xs&&... xs)
    { iN r, e;
      while ((r = f(std::forward<Xs>(xs)...)) == -1 &&
             ((e = errno) == EAGAIN || e == EINTR))
        // NOTE: errno may have been async-reset within g->y, so restore
        // it here before breaking
        if (!g || !g->y(ys)) { errno = e; break; }
      return r; }
};


}

#include "../end.hh"


#endif
