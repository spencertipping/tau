#ifndef tau_Φ_h
#define tau_Φ_h


#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>


#include "types.hh"
#include "phi.hh"
#include "Lambda.hh"
#include "gate.hh"
#include "utf9.hh"


#include "begin.hh"

namespace tau
{


void Φnb(uN fd)
{
  fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}


struct Φ;
struct Φf
{
  Φ     &f;
  λg     w;
  o9fdr  o;
  iN     we = 0;
  iN     wn = 1;  // NOTE: not always a real IO value; 1 means "not EOF, not error"
  iN     re = 0;
  iN     rn = 1;

  uN     wo = 0;  // write offset, to handle partial writes

  Φf(Φf &) = delete;
  Φf(Φ &f_, uN fd);
  ~Φf();

  uN   fd()  const { return o.fd; }
  bool eof() const { return !rn; }
  bool ra()  const { return !(rn == -1 && re == EAGAIN); }
  bool wa()  const { return !(wn == -1 && we == EAGAIN); }

  iN   rx()  const { return rn == -1 ? re : 0; }
  iN   wx()  const { return wn == -1 ? we : 0; }
};


struct Φ
{
  sletc        Φen = 64;  // number of events per epoll_wait call
  Λ           &l;
  iNc          fd;        // epoll control FD
  epoll_event  ev[Φen];   // inbound epoll event buffer


  Φ(Λ &l_) : l(l_), fd(epoll_create1(0))
    { A(fd != -1, "epoll_create1 failure " << errno); }

  ~Φ() { A(!close(fd), "~Φ close failed (fd leak) " << errno); }


  // TODO: why does this result in wakeups on every loop?
  Φ &operator<<(Φf &f)
    { epoll_event ev;
      ev.events   = EPOLLIN | EPOLLOUT | EPOLLET;
      ev.data.ptr = &f;
      A(epoll_ctl(fd, EPOLL_CTL_ADD, f.fd(), &ev) != -1,
        "epoll add failed " << errno);
      std::cerr << "added " << f.fd() << std::endl;
      return *this; }

  Φ &x(Φf &f)
    { epoll_event ev;
      ev.events   = EPOLLIN | EPOLLOUT | EPOLLET;
      ev.data.ptr = &f;
      A(epoll_ctl(fd, EPOLL_CTL_DEL, f.fd(), &ev) != -1,
        "epoll del failed " << errno);
      std::cerr << "removed " << f.fd() << std::endl;
      return *this; }


  uN operator()()
    { std::cerr << "epoll_wait" << std::endl;
      let n = epoll_wait(fd, ev, Φen, 1000);
      A(n != -1, "epoll_wait error " << errno);
      std::cerr << "epoll_wait returned " << n << std::endl;

      for (iN i = 0; i < n; ++i)
      { let f = Rc<Φf*>(ev[i].data.ptr);
        std::cerr << "waking up " << f->fd() << std::endl;
        f->rn = f->wn = 1;
        f->w.w(); }

      return n; }
};


inline Φf::Φf(Φ &f_, uN fd) : f(f_), w{f.l}, o{fd, rn, re}
{ Φnb(fd); f << *this; }

inline Φf::~Φf() { f.x(*this); }


template<class R, class F>
bool operator<<(φ<R, F> &f, Φf &r)
{
  while (!r.ra()) r.w.y(λs::ΦI);
  return f << r.o;
}


bool operator>>(i9 v, Φf &w)
{
  w.wo = 0;
  while (w.wo < v.size())
  {
    while (!w.wa()) w.w.y(λs::ΦO);
    if ((w.wn = write(w.fd(), v.begin() + w.wo, v.size() - w.wo)) == -1)
    { if ((w.we = errno) != EAGAIN)
        return false; }
    else
      w.wo += w.wn;
  }
  return true;
}


}

#include "end.hh"


#endif
