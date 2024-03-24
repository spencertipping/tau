#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include "tau-epoll.hh"
#include "../psi.hh"
#include "../begin.hh"

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
  sa.sa_handler = [](int) { if (current_t) current_t->sig(SIGTERM); exit(2); };
  sa.sa_flags   = 0;
  sigaction(SIGTERM, &sa, nullptr);

  sigemptyset(&sa.sa_mask);
  sa.sa_handler = [](int) { if (current_t) current_t->sig(SIGINT); };
  sa.sa_flags   = 0;
  sigaction(SIGINT, &sa, nullptr);
}


bool τe::reg(fd_t fd, bool r, bool w, bool et)
{
  A(efd != -1, "τe::reg(" << fd << ") on detached");
  r |= rgs.contains(fd);
  w |= wgs.contains(fd);
  if (rgs.contains(fd)) unreg(fd, true, false);
  if (wgs.contains(fd)) unreg(fd, false, true);

  epoll_event ev;
  ev.events  = (r ? EPOLLIN | EPOLLHUP : 0)
             | (w ? EPOLLOUT | EPOLLERR : 0)
             | (et ? EPOLLET : 0);
  ev.data.fd = nb(fd);
  if (epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev) == -1) return false;
  if (r) rgs[fd] = Sp<λgs>{new λgs{l_, l_}};
  if (w) wgs[fd] = Sp<λgs>{new λgs{l_, l_}};
  return true;
}


void τe::unreg(fd_t fd, bool r, bool w)
{
  if (r && rgs.contains(fd))
  {
    let g = rgs.at(fd);
    g->io.w(false);
    g->e.w (false);
    rgs.erase(fd);
  }

  if (w && wgs.contains(fd))
  {
    let g = wgs.at(fd);
    g->io.w(false);
    g->e.w (false);
    wgs.erase(fd);
  }

  // Explicitly deregister FD from epoll; see
  // https://idea.popcount.org/2017-03-20-epoll-is-fundamentally-broken-22/
  if (efd != -1 && !rgs.contains(fd) && !wgs.contains(fd))
    epoll_ctl(efd, EPOLL_CTL_DEL, fd, nullptr);
}


int τe::close(fd_t fd, bool r, bool w)
{
  // NOTE: sometimes we close only for reading or only for writing; close the
  // actual FD only if both directions are done.
  unreg(fd, r, w);
  return !rgs.contains(fd) && !wgs.contains(fd)
       ? ::close(fd)
       : 0;
}


uN τe::new_tid()
{
  uN r = ++tid;
  {
    Ul<Smu> l_(trs_m);
    while (trs.contains(r)) r = ++tid;  // make sure to claim a new ID
    trs.insert(r);
  }
  return r;
}


void τe::wake(uN tid)
{
  // Important: first mark the task as done by removing it from the set of
  // running tasks. Then wake epoll.
  { Ul<Smu> l_(trs_m); trs.erase(tid); }

  u64 x = 1;
  A(::write(wfd, &x, sizeof(x)) != -1, "τe::wake error");
}


bool τe::is_awake(uN tid)
{
  // This function is called after epoll_wait wakes up from the ::write() call
  // in wake(tid), so we need to check for set membership in trs. If we're
  // there, then we are _not_ done and must not consume a value from the
  // eventfd.
  bool w;
  { Sl<Smu> l_(trs_m); w = !trs.contains(tid); }

  if (w)
  {
    // We're done and can/must consume the eventfd value.
    u64 x;
    A(::read(wfd, &x, sizeof(x)) != -1,
      "τe::is_awake(" << tid << "): eventfd read error");
  }

  return w;
}


τe &τe::operator()(bool nonblock)
{
  A(efd != -1, "τe() on detached");

  // Important: if there's work to be done, then don't schedule any
  // blocking wait here; our goal is to check for realtime updates
  // and activate blocked λs, but pausing is a bad idea since we could
  // use the time with CPU-bound tasks.
  nonblock |= l_();

  // NOTE: rgs will never be empty because epoll_wait always listens to
  // wfd, which is an eventfd that allows it to be awoken from threads.
  while (!fin && now() < hn() &&
         (rgs.size() > 1 || !wgs.empty() || tp.tasks() || hn() != forever()))
  {
    epoll_event evs[256];

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
      if (rgs.contains(f))
      {
        let g = rat(f);
        let e = evs[i].events;
        // IN overrides HUP -- HUP doesn't preclude data being available
        if (e & (EPOLLIN | EPOLLHUP)) g->io.w(e & EPOLLIN);
        if (e & EPOLLERR)             g->e.w(true);
      }

      if (wgs.contains(f))
      {
        let g = wat(f);
        let e = evs[i].events;
        // ERR overrides OUT -- don't write if sigpipe
        if (e & (EPOLLERR | EPOLLOUT)) g->io.w(!(e & EPOLLERR));
        if (e & EPOLLERR)              g->e.w(true);
      }
    }

    // NOTE: continue to poll if we have more events, but don't block
    // for anything other than the first one -- we need to run another
    // Λ quantum to consume the events.
    nonblock = true;
  }

  while (now() >= hn()) l_.r(h_.top().l, λs::R), h_.pop();
  return *this;
}


τe &τe::go(bool nonblock, F<bool(τe&)> const &f)
{
  l_.step();
  while (f(*this)) (*this)(nonblock), l_.step();
  return *this;
}


void τe::sig(int s)
{
  for (let &f : sfs) (*f)(s);
}


}

#include "../end.hh"
