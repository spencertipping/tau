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

  Λt() {}
  Λt(λf &&f) : l(λ(std::move(f))), s(λs::S) {}

  Λt &operator=(Λt &&x)
    { l = std::move(x.l);
      s = x.s;
      return *this; }

  bool done()     const { return l.done(); }
  bool runnable() const { return s == λs::R; }
};


// NOTE: managed λs should yield out with Λ.y, not λy() defined by lambda.hh
struct Λ
{
protected:
  M<λi, Λt> ls;     // all λs
  S<λi>     rs;     // run set
  λi        ni{0};  // next λi (always nonzero for managed λ)
  λi        ri{0};  // currently running λi (0 = main thread)
  ΣΘΔ       qΘ;     // quantum time measurement


public:
  Λ(Λ &)  = delete;
  Λ(Λ &&) = delete;
  Λ() {}

  bool e (λi i) const { return ls.contains(i); }
  λs   si(λi i) const { return ls.at(i).s; }
  uN   n ()     const { return ls.size(); }
  λi   i ()     const { return ri; }
  bool z ()     const { return !ri; }

  λi   c(λf &&f)
    { let i = ιi(ni, ls);
      ls[i] = Λt(std::move(f));
      r(i, λs::R);
      return  i; }

  Λ  &x(λi i)
    { A(ri != i, "self λx");
      A(e(i), "λx !e");
      ls.erase(i);
      return *this; }

  Λ  &y(λs s)  // yield currently-running λ with specified yield state
    { A(!z(), "Λy from main thread");
      r(ri, s);
      λy();
      return *this; }

  Λ  &r(λi i, λs s = λs::R)  // set λi to runnable, or other state
    { if (!e(i)) return *this;
      auto &l = ls.at(i);
      l.s = s;
      if (l.runnable()) rs.insert(i);
      return *this; }

  Λt const &operator[](λi i) const { return ls.at(i); }

  Λ &operator<<(λi i)  // run λi for one quantum
    { A(z(), "non-root Λ<<");
      auto &l = ls.at(ri = i);
      qΘ.start();
      l.l();
      ri = 0;
      if (l.l.done()) ls.erase(i);
      qΘ.stop();
      return *this; }

  λi operator()()  // find next λi to run
    { for (let i : rs)
        if (e(i) && si(i) == λs::R) { rs.erase(i); return i; }
      return 0; }

  Λ &go()  // run as long as there is stuff to do
    { for (λi t; (t = (*this)()); *this << t);
      return ry(); }

  Λ &ry()  // resume all explicitly-yielded λs
    { for (auto &[i, s] : ls) if (s.s == λs::Y) r(i);
      return *this; }

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
