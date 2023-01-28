#ifndef τ²epoll_h
#define τ²epoll_h


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
#include "Lambda.hh"
#include "gate.hh"

#include "tau-common.hh"


#include "begin.hh"

namespace τ
{


// TODO: rework most of this logic; it's pretty convoluted


inline void τnb(uN fd)
{
  fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}


template<class O>
struct τf  // FD container for τ, behavior determined by O (an o9)
{
  τ  &t;
  λg  w;
  iN  rn = 1;  // NOTE: not always a real IO value; 1 means "not EOF, not error"
  iN  re = 0;
  iN  wn = 1;
  iN  we = 0;
  uN  wo = 0;  // write offset, to handle partial writes
  uN  ep = 0;  // errno from epoll_ctl add, if any
  O   o;       // important: must be final field

  τf(τf const&) = delete;
  τf(τf&&)      = delete;

  template<class... T> τf(τ &f_, T...);
  ~τf();

  void init();

  uN   fd()  const { return o.fd; }
  bool eof() const { return !rn; }
  bool ra()  const { return !(rn == -1 && re == EAGAIN); }
  bool wa()  const { return !(wn == -1 && we == EAGAIN); }

  τf  &reset()     { rn = wn = 1; return *this; }

  iN   rx()  const { return rn == -1 ? re : 0; }
  iN   wx()  const { return wn == -1 ? we : 0; }
};


O &operator<<(O&, τ&);

template<class fO>
O &operator<<(O&, τf<fO> const&);


struct τ : public τb
{
  sletc τen = 256;  // number of events per epoll_wait call

  iNc             efd;      // epoll control FD
  M<uN, S<void*>> fs;       // who's listening for each FD
  S<void*>        nfs;      // non-monitored files
  epoll_event     ev[τen];  // inbound epoll event buffer


  τ(τ&)  = delete;
  τ(τ&&) = delete;
  τ() : τb(), efd(epoll_create1(0))
    { signal(SIGPIPE, SIG_IGN);
      A(efd != -1, "epoll_create1 failure " << errno); }

  ~τ() { A(!close(efd), "~τ close failed (fd leak) " << errno); }

  constexpr bool is_async() { return false; }


  template<class O>
  bool operator<<(τf<O> &f)
    { epoll_event ev;
      ev.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLET;
      let c = ev.data.fd = f.fd();
      let r = fs.contains(c) || epoll_ctl(efd, EPOLL_CTL_ADD, c, &ev) != -1;
      if (r) fs[c].emplace(&f);
      else   nfs.emplace(&f);
      return r; }

  template<class O>
  bool x(τf<O> &f)
    { nfs.erase(&f);
      let c = f.fd();
      if (fs.contains(c))
      { auto &s = fs.at(c);
        s.erase(&f);
        if (!s.empty()) return true;
        fs.erase(c); }
      return true; }


  τ &operator()()
    { let t = now();
      if (t < hn() && (!fs.empty() || hn() < forever()))
      { let dt = (hn() - t) / 1ms;
        let n  = epoll_wait(efd, ev, τen, std::min(dt, Sc<decltype(dt)>(Nl<int>::max())));
        A(n != -1, "epoll_wait error " << errno);
        for (iN i = 0; i < n; ++i)
          for (let f : fs.at(ev[i].data.fd))
            // NOTE: mistyping here is OK, as we just want to issue wakeups
            Rc<τf<void*>*>(f)->reset().w.w(); }
      while (now() >= hn()) l.r(h.top().l), h.pop();
      return *this; }

  operator bool() const
    { return !fs.empty() || !nfs.empty() || hn() != forever(); }

  τ &go_async() { A(0, "τ is not async"); return *this; }
  τ &go(F<bool(τ&)> const &f = [](τ &f) { return Sc<bool>(f); })
    { l.go();
      while (f(*this)) (*this)(), l.go();
      return *this; }
};


template<class O>
template<class... T>
inline τf<O>::τf(τ &t_, T... xs) : t(t_), w{t.l}, o{rn, re, xs...}
{
  init();
}


template<class T, class O>
bool τread(T &x, τf<O> &r)
{
  TODO("FIXME: τread() needs operator<< to be updated");
  while (1)
  {
    if      (x << r.o)         return true;
    else if (!r.ep && !r.ra()) r.w.y(λs::τI);
    else                       return false;
  }
}


O &operator<<(O &s, τΘ const &h)
{
  return s << "τΘ:" << h.h << ":" << h.l;
}

template<class fO>
O &operator<<(O &s, τf<fO> const &f)
{
  return s << "τf[" << f.o.fd << ":" << f.ep
           << " r=" << f.rn << " " << f.re
           << " w=" << f.wn << " " << f.we << "]";
}

O &operator<<(O &s, τ &t)
{
  V<τΘ> hs;
  while (!t.h.empty()) hs.push_back(t.h.top()), t.h.pop();
  for (let &h : hs) t.h.push(h);
  s << "τ efd=" << t.efd << " Θ=";
  for (let &h : hs) s << h << " ";
  s << "; fds=";
  for (let &[fd, l] : t.fs) s << fd << ":" << l.size() << " ";
  s << "; nfs=" << t.nfs.size();
  return s << std::endl << t.l << std::endl;
}


}

#include "end.hh"


#endif
