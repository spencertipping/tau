extern "C"
{
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <unistd.h>
}


auto const constexpr MAX_EVENTS = 8;


void set_nonblock(int fd)
{
  int flags = fcntl(fd, F_GETFD);
  fcntl(fd, F_SETFD, flags | O_NONBLOCK);
}


int main()
{
  epoll_event ev, events[MAX_EVENTS];
  int epollfd = epoll_create1(0);
  if (epollfd == -1)
  {
    perror("epoll_create1");
    return 1;
  }

  ev.events  = EPOLLIN | EPOLLET;
  ev.data.fd = 0;
  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, 0, &ev) == -1)
  {
    perror("epoll_ctl: 0");
    return 1;
  }

  ev.events  = EPOLLOUT | EPOLLET;
  ev.data.fd = 1;
  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, 1, &ev) == -1)
  {
    perror("epoll_ctl: 1");
    return 1;
  }

  bool in_ok  = false;
  bool out_ok = false;

  set_nonblock(0);
  set_nonblock(1);

  for (;;)
  {
    int nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
    printf("nfds = %d\n", nfds);

    if (nfds == -1)
    {
      perror("epoll_wait");
      return 1;
    }

    for (int i = 0; i < nfds; ++i)
    {
      if (events[i].data.fd == 0) in_ok  = true;
      if (events[i].data.fd == 1) out_ok = true;
    }

    if (in_ok && out_ok)
    {
      // Splice data
      splice(0, NULL, 1, NULL, 8192, SPLICE_F_NONBLOCK);
    }
  }

  return 0;
}
