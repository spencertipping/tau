#include "arch.hh"
#if τhas_epoll

#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include "tau-epoll.hh"
#include "begin.hh"

namespace τ
{


static τe *current_t = nullptr;


void τe::init_signals()
{
  // Keep track of the current signal receiver (not elegant, but it works)
  current_t = this;

  // Important: ignore SIGPIPE so we can catch it as an error on FD ops
  signal(SIGPIPE, SIG_IGN);

  // Effectively ignore all child exits; we don't care about the PIDs
  // as much as we do the FIFO output that they manage.
  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = [](int _) { while (waitpid(-1, &_, WNOHANG) > 0); };
  sa.sa_flags   = 0;
  sigaction(SIGCHLD, &sa, nullptr);

  // Terminate all children on SIGTERM or SIGINT
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = [](int) { if (current_t) current_t->term(); exit(2); };
  sa.sa_flags   = 0;
  sigaction(SIGTERM, &sa, nullptr);
  sigaction(SIGINT,  &sa, nullptr);
}


void τe::detach()
{
  if (efd != -1) ::close(efd);
  efd = -1;

  // Close FDs without removing from epoll
  for (let &[fd, g] : gs)
  {
    ::close(fd);
    g->r.w(false);
    g->w.w(false);
    g->e.w(false);
    delete g;
  }
  gs.clear();

  // Clear out any time-queue entries
  while (!h_.empty()) h_.pop();

  // Also delete all child pids, as they are probably managed by our parent
  pids.clear();
}


bool τe::reg(fd_t fd, bool r, bool w)
{
  if (gs.contains(fd))
  {
    epoll_ctl(efd, EPOLL_CTL_DEL, fd, nullptr);
    delete gs.at(fd);
    gs.erase(fd);
  }

  epoll_event ev;
  ev.events  = (r ? EPOLLIN | EPOLLHUP : 0) | (w ? EPOLLOUT | EPOLLERR : 0) | EPOLLET;
  ev.data.fd = nb(fd);
  if (epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev) == -1) return false;
  gs[fd] = new λgs{r, w, l_, l_, l_};
  return true;
}


int τe::close(fd_t fd)
{
  if (gs.contains(fd))
  {
    let g = gs.at(fd);
    g->r.w(false);
    g->w.w(false);
    delete g;
    gs.erase(fd);
  }

  // Explicitly deregister FD from epoll; see
  // https://idea.popcount.org/2017-03-20-epoll-is-fundamentally-broken-22/
  epoll_ctl(efd, EPOLL_CTL_DEL, fd, nullptr);

  return ::close(fd);
}


int τe::fork()
{
  let r = ::fork();
  if (r) pids.insert(r);
  return r;
}


void τe::term()
{
  for (let p : pids) kill(p, SIGTERM);
  pids.clear();

  fin = true;
}


τe &τe::operator()(bool nonblock)
{
  while (!fin && now() < hn() && (!gs.empty() || hn() != forever()))
  {
    epoll_event evs[16];

    // We may need to retry if epoll is interrupted by SIGCHLD
  epoll:
    let dt = (hn() - now()) / 1ms;
    let n  = epoll_wait(efd, evs, sizeof(evs) / sizeof(epoll_event),
                        nonblock ? 0 : std::min(dt, Sc<decltype(dt)>(Nl<int>::max())));
    if (!fin && n == -1 && errno == EINTR) goto epoll;

    A(fin || n != -1, "epoll_wait error " << errno);
    if (n <= 0) break;

    for (int i = 0; i < n; ++i)
    {
      let f = evs[i].data.fd;
      if (gs.contains(f))
      {
        let g = at(f);
        let e = evs[i].events;

        // ERR overrides OUT -- don't write if sigpipe
        // IN overrides HUP -- HUP doens't preclude data being available
        if (e & (EPOLLIN  | EPOLLHUP)) g->r.w(  e & EPOLLIN);
        if (e & (EPOLLERR | EPOLLOUT)) g->w.w(!(e & EPOLLERR));
        if (e & EPOLLERR)              g->e.w(true);
      }
    }

    // NOTE: continue to poll if we have more events, but don't block
    // for anything other than the first one -- we need to run another
    // Λ quantum to consume the events.
    nonblock = true;
  }

  while (now() >= hn()) l_.r(h_.top().l), h_.pop();
  return *this;
}


τe &τe::clear()
{
  qs.clear();
  V<fd_t> fds;
  for (let &[fd, g] : gs) fds.push_back(fd);
  for (let x : fds) close(x);
  while (!h_.empty()) h_.pop();
  return *this;
}


}

#include "end.hh"
#endif
