#ifndef τΛh
#define τΛh


#include "arch.hh"
#include "debug.hh"
#include "types.hh"
#include "shd.hh"

#include "lambda.hh"


#include "begin.hh"

namespace τ
{


// A single Λ task, which is a λ plus some metadata
struct Λt
{
  λ  l;
  λs s;

  Λt(λf &&f) : l(λ(std::move(f))), s(λs::S) {}

  bool done()     const { return l.done(); }
  bool runnable() const { return s == λs::R; }
};


// NOTE: managed λs should yield out with Λ.y, not λy() defined by lambda.hh
struct Λ
{
  Λ(Λ &)  = delete;
  Λ(Λ &&) = delete;
  Λ() {}

  // This deserves some explanation. ~Λ causes ~λ, which causes ~λf, which
  // causes ~ξ, which causes λg::w(), which attempts to wake more λs.
  //
  // Well, by that point our data structures are in some undefined state
  // of deallocation; it's bad. So we set fin here to indicate that the
  // world is ending, nothing matters from here.
  ~Λ() { fin = true; }

  bool e (λi i) const { return ls.contains(i); }
  λi   i ()     const { return ri; }

  λi c(λf &&f)
    { if (fin) return 0;
      let i = ιi(ni, ls);
      ls[i].reset(new Λt(std::move(f)));
      r(i, λs::R);
      return  i; }

  Λ &x(λi i)
    { if (fin) return *this;
      A(ri != i, "self λx");
      A(e(i), "λx !e");
      ls.erase(i);
      return *this; }

  Λ &y(λs s)  // yield currently-running λ with specified yield state
    { if (fin) return *this;
      A(i(), "Λy from main thread");
      r(ri, s);
      λy();
      return *this; }

  Λ &r(λi i, λs s = λs::R)  // set λi to runnable, or other state
    { if (fin || !e(i)) return *this;
      auto &l = *ls.at(i);
      l.s = s;
      if (l.runnable()) rs.insert(i);
      return *this; }

  Λt const &operator[](λi i) const
    { A(!fin, "Λ[] fin");
      return *ls.at(i); }


  Λ &operator<<(λi i)  // run λi for one quantum
    { if (fin) return *this;
      A(!ri, "Λ<< from non-main thread");
      auto &l = *ls.at(ri = i);
      qΘ.start();
      l.l();
      ri = 0;
      if (l.l.done()) ls.erase(i);
      qΘ.stop();
      return *this; }

  λi operator()()  // find next λi to run
    { if (fin) return 0;
      for (let i : rs)
        if (e(i) && (*this)[i].runnable()) { rs.erase(i); return i; }
      return 0; }

  Λ &go()  // run as long as there is stuff to do
    { for (λi t; (t = (*this)()); *this << t);
      return ry(); }

  Λ &ry()  // resume all explicitly-yielded λs
    { for (auto &[i, s] : ls) if (s->s == λs::Y) r(i);
      return *this; }


protected:
  M<λi, Sp<Λt>> ls;           // all λs
  S<λi>         rs;           // run set
  ΣΘΔ           qΘ;           // quantum time measurement
  λi            ni  = 0;      // next λi (always nonzero for managed λ)
  λi            ri  = 0;      // currently running λi (0 = main thread)
  bool          fin = false;  // we're done; ignore all requests

#if τdebug_iostream
  friend O &operator<<(O &s, Λ &l);
#endif
};


#if τdebug_iostream
O &operator<<(O &s, Λt const &l)
{
  return s << "λ" << l.s << (l.done() ? "=" : "");
}

O &operator<<(O &s, Λ &l)
{
  s << "Λ λ=" << l.i() << " r="; for (let i : l.rs) s << i << " "; s << std::endl;
  for (let &[k, v] : l.ls) s << "  " << k << "\t" << v << std::endl;
  return s;
}
#endif


}

#include "end.hh"


#endif
