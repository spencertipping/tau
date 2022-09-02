#ifndef tau_Φ_h
#define tau_Φ_h


// TODO: have a separate version for emscripten


#include <algorithm>

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
  iN     rn = 1;  // NOTE: not always a real IO value; 1 means "not EOF, not error"
  iN     re = 0;
  iN     wn = 1;
  iN     we = 0;
  uN     wo = 0;  // write offset, to handle partial writes
  bool   ep = false;

  Φf(Φ &f_, uN fd, u32 s = 1 << ζb0 - 1);
  ~Φf();

  uN   fd()  const { return o.fd; }
  bool eof() const { return !rn; }
  bool ra()  const { return !(rn == -1 && re == EAGAIN); }
  bool wa()  const { return !(wn == -1 && we == EAGAIN); }

  Φf &reset()      { rn = wn = 1; return *this; }

  iN   rx()  const { return rn == -1 ? re : 0; }
  iN   wx()  const { return wn == -1 ? we : 0; }
};


struct ΦΘ
{
  Θp h;
  λi l;
  bool operator<(ΦΘ const &x) const { return h > x.h; }
};


struct Φ
{
  sletc        Φen = 256;  // number of events per epoll_wait call
  Λ           &l;
  iNc          fd;         // epoll control FD
  uN           fds = 0;    // number of managed FDs
  epoll_event  ev[Φen];    // inbound epoll event buffer
  PQ<ΦΘ>       h;


  Φ(Λ &l_) : l(l_), fd(epoll_create1(0))
    { A(fd != -1, "epoll_create1 failure " << errno); }

  ~Φ() { A(!close(fd), "~Φ close failed (fd leak) " << errno); }


  bool operator<<(Φf &f)
    { epoll_event ev;
      ev.events   = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLET;
      ev.data.ptr = &f;
      ++fds;
      return epoll_ctl(fd, EPOLL_CTL_ADD, f.fd(), &ev) != -1; }

  bool x(Φf &f)
    { --fds;
      return !f.ep
          || epoll_ctl(fd, EPOLL_CTL_DEL, f.fd(), nullptr) != -1; }


  Φ &Θ(Θp t)
    { while (now() < t) { h.push(ΦΘ{t, l.i()}); l.y(λs::Θ); }
      return *this; }

  Θp hn() const { return h.empty() ? forever() : h.top().h; }


  Φ &operator()()
    { let t = now();
      if (t < hn())
      { let dt = (hn() - t) / 1ms;
        let n  = epoll_wait(fd, ev, Φen, std::min(dt, Sc<decltype(dt)>(Nl<int>::max())));
        A(n != -1, "epoll_wait error " << errno);
        for (iN i = 0; i < n; ++i) Rc<Φf*>(ev[i].data.ptr)->reset().w.w(); }
      while (now() >= hn()) l.r(h.top().l), h.pop();
      return *this; }

  Φ &go(F<bool(Φ&)> const &f = [](Φ &f) { return f.fds || f.hn() != forever(); })
    { l.go();
      while (f(*this)) (*this)(), l.go();
      return *this; }
};


inline Φf::Φf(Φ &f_, uN fd, u32 s) : f(f_), w{f.l}, o{fd, rn, re, s}
{ Φnb(fd); ep = f << *this; }

inline Φf::~Φf()
{ f.x(*this); }


template<class R, class W, class F>
bool operator<<(φ<R, W, F> &f, Φf &r)
{
  while (1)
  {
    if      (f << r.o)        return true;
    else if (r.ep && !r.ra()) r.w.y(λs::ΦI);
    else                      return false;
  }
}


bool operator>>(i9 v, Φf &w)
{
  w.wo = 0;
  while (w.wo < v.size())
  {
    if (w.ep) while (!w.wa()) w.w.y(λs::ΦO);
    if ((w.wn = write(w.fd(), v.begin() + w.wo, v.size() - w.wo)) == -1)
    { if ((w.we = errno) != EAGAIN)
        return false; }
    else
      w.wo += w.wn;
  }
  return true;
}


#ifdef tau_debug_iostream
O &operator<<(O &s, ΦΘ const &h)
{
  return s << "ΦΘ:" << h.h << ":" << h.l;
}

O &operator<<(O &s, Φf const &f)
{
  return s << "<<Φf TODO";
}

O &operator<<(O &s, Φ &f)
{
  V<ΦΘ> hs;
  while (!f.h.empty()) hs.push_back(f.h.top()), f.h.pop();
  for (auto &h : hs) f.h.push(h);
  s << "Φ fd=" << f.fd << " fds=" << f.fds << " Θ=";
  for (auto &h : hs) s << h << " ";
  s << std::endl;
  return s << f.l << std::endl;
}
#endif


}

#include "end.hh"


#endif
