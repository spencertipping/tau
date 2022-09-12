#ifndef tau_Φ_h
#define tau_Φ_h


// TODO: have a separate version for emscripten

// TODO: cap the wait-time for files and other non-epollables
// so we revisit and awaken them


#include <algorithm>

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <strings.h>
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


#if tau_debug_iostream
O &operator<<(O&, Φ&);
#endif


template<class O>
struct Φf
{
  Φ  &f;
  λg  w;
  iN  rn = 1;  // NOTE: not always a real IO value; 1 means "not EOF, not error"
  iN  re = 0;
  iN  wn = 1;
  iN  we = 0;
  uN  wo = 0;  // write offset, to handle partial writes
  uN  ep = 0;  // errno from epoll_ctl add, if any
  O   o;       // important: must be final field

  Φf(Φf const&) = delete;
  Φf(Φf&&)      = delete;
  Φf(Φ &f_, uN fd, u32 s = 1 << ζb0 - 1);
  ~Φf();

  uN   fd()  const { return o.fd; }
  bool eof() const { return !rn; }
  bool ra()  const { return !(rn == -1 && re == EAGAIN); }
  bool wa()  const { return !(wn == -1 && we == EAGAIN); }

  Φf  &reset()     { rn = wn = 1; return *this; }

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
  sletc Φen = 256;  // number of events per epoll_wait call

  Λ               l;
  iNc             efd;        // epoll control FD
  M<uN, S<void*>> fs;         // who's listening for each FD
  epoll_event     ev[Φen];    // inbound epoll event buffer
  PQ<ΦΘ>          h;          // upcoming timed λs
  Θp              t0 = now();


  Φ(Φ&)  = delete;
  Φ(Φ&&) = delete;
  Φ() : efd(epoll_create1(0))
    { signal(SIGPIPE, SIG_IGN);
      A(efd != -1, "epoll_create1 failure " << errno); }

  ~Φ() { A(!close(efd), "~Φ close failed (fd leak) " << errno); }


  template<class O>
  bool operator<<(Φf<O> &f)
    { epoll_event ev;
      ev.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLET;
      let c = ev.data.fd = f.fd();
      let r = fs.contains(c) || epoll_ctl(efd, EPOLL_CTL_ADD, c, &ev) != -1;
      if (r) fs[c].emplace(&f);
      return r; }

  template<class O>
  bool x(Φf<O> &f)
    { if (f.ep) return true;
      let c = f.fd();
      auto &s = fs.at(c);
      s.erase(&f);
      if (!s.empty()) return true;
      fs.erase(c);
      return !close(c); }


  Φ &Θ(Θp t)
    { while (now() < t) { h.push(ΦΘ{t, l.i()}); l.y(λs::Θ); }
      return *this; }

  Θp hn() const { return h.empty() ? forever() : h.top().h; }

  ΔΘ dt() const { return now() - t0; }


  Φ &operator()()
    { let t = now();
      if (t < hn())
      { let dt = (hn() - t) / 1ms;
        let n  = epoll_wait(efd, ev, Φen, std::min(dt, Sc<decltype(dt)>(Nl<int>::max())));
        A(n != -1, "epoll_wait error " << errno);
        for (iN i = 0; i < n; ++i)
          for (let f : fs.at(ev[i].data.fd))
            // NOTE: mistyping here is OK, as we just want to issue wakeups
            Rc<Φf<o9fdr>*>(f)->reset().w.w(); }
      while (now() >= hn()) l.r(h.top().l), h.pop();
      return *this; }

  Φ &go(F<bool(Φ&)> const &f = [](Φ &f) { return !f.fs.empty() || f.hn() != forever(); })
    { l.go();
      while (f(*this)) (*this)(), l.go();
      return *this; }
};


template<>
inline Φf<o9fdr>::Φf(Φ &f_, uN fd_, u32 s) : f(f_), w{f.l}, o{fd_, rn, re, s}
{ Φnb(fd_); ep = f << *this ? 0 : errno; }

template<>
inline Φf<o9acc>::Φf(Φ &f_, uN fd_, u32 s) : f(f_), w{f.l}, o{fd_, rn, re}
{ Φnb(fd_); ep = f << *this ? 0 : errno; }

template<class O>
inline Φf<O>::~Φf()
{ f.x(*this); }


template<class R, class W, class F, class O>
bool operator<<(φ<R, W, F> &f, Φf<O> &r)
{
  while (1)
  {
    if      (f << r.o)         return true;
    else if (!r.ep && !r.ra()) r.w.y(λs::ΦI);
    else                       return false;
  }
}


bool operator>>(i9 v, Φf<o9fdr> &w)
{
  w.wo = 0;
  while (w.wo < v.size())
  {
    if (!w.ep) while (!w.wa()) w.w.y(λs::ΦO);
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

template<class fO>
O &operator<<(O &s, Φf<fO> const &f)
{
  return s << "Φf[" << f.o.fd << ":" << f.ep
           << " r=" << f.rn << " " << f.re
           << " w=" << f.wn << " " << f.we << "]";
}

O &operator<<(O &s, Φ &f)
{
  V<ΦΘ> hs;
  while (!f.h.empty()) hs.push_back(f.h.top()), f.h.pop();
  for (let &h : hs) f.h.push(h);
  s << "Φ efd=" << f.efd << " Θ=";
  for (let &h : hs) s << h << " ";
  s << "; fds=";
  for (let &[fd, l] : f.fs) s << fd << ":" << l.size() << " ";
  return s << std::endl << f.l << std::endl;
}
#endif


}

#include "end.hh"


#endif
