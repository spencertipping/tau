#ifndef τΦ_epoll_h
#define τΦ_epoll_h


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
#include "utf9.hh"

#include "Phi-common.hh"


#include "begin.hh"

namespace τ
{


inline void Φnb(uN fd)
{
  fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}


template<class O>
struct Φf  // FD container for Φ, behavior determined by O (an o9)
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

  template<class... T> Φf(Φ &f_, T...);
  ~Φf();

  void init();

  uN   fd()  const { return o.fd; }
  bool eof() const { return !rn; }
  bool ra()  const { return !(rn == -1 && re == EAGAIN); }
  bool wa()  const { return !(wn == -1 && we == EAGAIN); }

  Φf  &reset()     { rn = wn = 1; return *this; }

  iN   rx()  const { return rn == -1 ? re : 0; }
  iN   wx()  const { return wn == -1 ? we : 0; }
};


#if τdebug_iostream
O &operator<<(O&, Φ&);

template<class fO>
O &operator<<(O&, Φf<fO> const&);
#endif


struct Φ : public Φb
{
  sletc Φen = 256;  // number of events per epoll_wait call

  iNc             efd;      // epoll control FD
  M<uN, S<void*>> fs;       // who's listening for each FD
  S<void*>        nfs;      // non-monitored files
  epoll_event     ev[Φen];  // inbound epoll event buffer


  Φ(Φ&)  = delete;
  Φ(Φ&&) = delete;
  Φ() : Φb(), efd(epoll_create1(0))
    { signal(SIGPIPE, SIG_IGN);
      A(efd != -1, "epoll_create1 failure " << errno); }

  ~Φ() { A(!close(efd), "~Φ close failed (fd leak) " << errno); }

  constexpr bool is_async() { return false; }


  template<class O>
  bool operator<<(Φf<O> &f)
    { epoll_event ev;
      ev.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLET;
      let c = ev.data.fd = f.fd();
      let r = fs.contains(c) || epoll_ctl(efd, EPOLL_CTL_ADD, c, &ev) != -1;
      if (r) fs[c].emplace(&f);
      else   nfs.emplace(&f);
      return r; }

  template<class O>
  bool x(Φf<O> &f)
    { nfs.erase(&f);
      let c = f.fd();
      if (fs.contains(c))
      { auto &s = fs.at(c);
        s.erase(&f);
        if (!s.empty()) return true;
        fs.erase(c); }
      return true; }


  Φ &operator()()
    { let t = now();
      if (t < hn() && (!fs.empty() || hn() < forever()))
      { let dt = (hn() - t) / 1ms;
        let n  = epoll_wait(efd, ev, Φen, std::min(dt, Sc<decltype(dt)>(Nl<int>::max())));
        A(n != -1, "epoll_wait error " << errno);
        for (iN i = 0; i < n; ++i)
          for (let f : fs.at(ev[i].data.fd))
            // NOTE: mistyping here is OK, as we just want to issue wakeups
            Rc<Φf<void*>*>(f)->reset().w.w(); }
      while (now() >= hn()) l.r(h.top().l), h.pop();
      return *this; }

  operator bool() const
    { return !fs.empty() || !nfs.empty() || hn() != forever(); }

  Φ &go_async() { A(0, "Φ is not async"); return *this; }
  Φ &go(F<bool(Φ&)> const &f = [](Φ &f) { return Sc<bool>(f); })
    { l.go();
      while (f(*this)) (*this)(), l.go();
      return *this; }
};


template<class O>
template<class... T>
inline Φf<O>::Φf(Φ &f_, T... xs) : f(f_), w{f.l}, o{rn, re, xs...}
{
  init();
}


// FIXME: we should allow non-φ LHS for this function
// so we can read into π heap
template<class R, class W, class F, class O>
bool operator<<(φ_<R, W, F> &f, Φf<O> &r)
{
  while (1)
  {
    if      (f << r.o)         return true;
    else if (!r.ep && !r.ra()) r.w.y(λs::ΦI);
    else                       return false;
  }
}


#ifdef τdebug_iostream
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
  s << "; nfs=" << f.nfs.size();
  return s << std::endl << f.l << std::endl;
}
#endif


}

#include "end.hh"


#endif
