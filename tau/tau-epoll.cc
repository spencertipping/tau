#include "arch.hh"
#if τhas_epoll

#include "tau-epoll.hh"
#include "begin.hh"

namespace τ
{


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


τe &τe::operator()(bool nonblock)
{
  while (now() < hn() && (!gs.empty() || hn() != forever()))
  {
    epoll_event evs[16];

    // We may need to retry if epoll is interrupted by SIGCHLD
  epoll:
    let dt = (hn() - now()) / 1ms;
    let n  = epoll_wait(efd, evs, sizeof(evs) / sizeof(epoll_event),
                        nonblock ? 0 : std::min(dt, Sc<decltype(dt)>(Nl<int>::max())));
    if (n == -1 && errno == EINTR) goto epoll;

    A(n != -1, "epoll_wait error " << errno);
    if (!n) break;

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


bool τe::reg(fd_t fd, bool r, bool w)
{
  if (gs.contains(fd))
  {
    epoll_ctl(efd, EPOLL_CTL_DEL, fd, nullptr);
    delete gs.at(fd);
    gs.erase(fd);
  }

  epoll_event ev;
  ev.events  = (r ? EPOLLIN  | EPOLLHUP : 0)
             | (w ? EPOLLOUT | EPOLLERR : 0) | EPOLLET;
  ev.data.fd = nb(fd);
  if (epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev) == -1) return false;
  gs[fd] = new λgs{r, w, l_, l_, l_};
  return true;
}


}

#include "end.hh"
#endif
