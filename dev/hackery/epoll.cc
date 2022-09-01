#include <iostream>

#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>


using namespace std;


int main()
{
  epoll_event q[64];

  int fd = epoll_create1(0);
  epoll_event ev;
  ev.events  = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLET;
  ev.data.fd = 0;
  if (fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK) == -1)
    perror("set nonblock");

  if (epoll_ctl(fd, EPOLL_CTL_ADD, 0, &ev) == -1)
    perror("epoll add");

  ev.events  = EPOLLOUT | EPOLLOUT | EPOLLERR | EPOLLET;
  ev.data.fd = 1;
  if (fcntl(1, F_SETFL, fcntl(1, F_GETFL) | O_NONBLOCK) == -1)
    perror("set nonblock");

  if (epoll_ctl(fd, EPOLL_CTL_ADD, 1, &ev) == -1)
    perror("epoll add");

  char buf[8192];
  while (1)
  {
    int en = epoll_wait(fd, q, 64, 1000);
    cerr << "got " << en << " fd(s)" << endl;

    for (int i = 0; i < en; ++i)
      cerr << "  fd[" << i << "] = " << q[i].data.fd << endl;

    for (int i = 0; i < en; ++i)
      if (q[i].data.fd == 0)
        for (int n; (n = read(q[i].data.fd, buf, 8192)) > 0;)
          write(1, buf, n);
  }
}
