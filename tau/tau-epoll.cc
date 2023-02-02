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
  return ::close(fd);
}


τe &τe::operator()(bool nonblock)
{
  while (now() < hn() && (!gs.empty() || hn() != forever()))
  {
    epoll_event evs[16];
    let dt = (hn() - now()) / 1ms;
    let n  = epoll_wait(efd, evs, sizeof(evs) / sizeof(epoll_event),
                        nonblock ? 0 : std::min(dt, Sc<decltype(dt)>(Nl<int>::max())));

    A(n != -1, "epoll_wait error " << errno);
    if (!n) break;

    for (int i = 0; i < n; ++i)
    {
      let f = evs[i].data.fd;
      if (gs.contains(f))
      {
        let g = at(f);
        if (evs[i].events & EPOLLIN)  g->r.w(true);
        if (evs[i].events & EPOLLOUT) g->w.w(true);
        if (evs[i].events & EPOLLERR) g->w.w(false);
        if (evs[i].events & EPOLLHUP) g->r.w(false);
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


τe::λgs *τe::at(fd_t fd)
{
  if (!gs.contains(fd))
  {
    epoll_event ev;
    ev.events  = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLET;
    ev.data.fd = nb(fd);
    if (epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev) == -1) return nullptr;
    return gs[fd] = new λgs{l_, l_};
  }
  return gs.at(fd);
}


}

#include "end.hh"
#endif
