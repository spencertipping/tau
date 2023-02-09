#include "Lambda.hh"
#include "begin.hh"

namespace τ
{


// TODO: introduce locally-scoped exception type


λi Λ::c(λf &&f)
{
  // TODO: wrap f in a try/catch here, so we can destroy λs
  if (fin) return 0;
  let i = ιi(ni, ls);
  ls[i].reset(new Λt(std::move(f)));
  r(i, λs::R);
  return i;
}

Λ &Λ::x(λi i)
{
  // FIXME: add a "being destroyed" state so we can throw/catch to
  // fully unwind the λ stack
  if (fin) return *this;
  A(ri != i, "self λx");  // FIXME: self-destroy is acceptable now
  ls.erase(i);
  return *this;
}


Λ &Λ::y(λs s)
{
  if (fin) return *this;
  A(i(), "Λ::y from main thread");
  r(ri, s);
  λy();

  // TODO: throw exception here if the λ is being destroyed
  return *this;
}

Λ &Λ::r(λi i, λs s)
{
  if (fin || !e(i)) return *this;
  auto &l = *ls.at(i);
  l.s = s;
  if (l.runnable()) rs.insert(i);
  return *this;
}


Λ &Λ::operator<<(λi i)
{
  if (fin) return *this;
  A(!ri, "Λ<< from non-main thread");
  rs.erase(i);
  auto &l = *ls.at(ri = i);
  qΘ.start();
  l.run();
  ri = 0;
  if (l.done()) ls.erase(i);
  qΘ.stop();
  return *this;
}


λi Λ::operator()() const
{
  if (fin) return 0;
  for (let i : rs) if (e(i) && ls.at(i)->runnable()) return i;
  return 0;
}

Λ &Λ::go()
{
  for (λi t; (t = (*this)()); *this << t);
  for (auto &[i, s] : ls) if (s->s == λs::Y) r(i);
  return *this;
}


O &operator<<(O &s, Λt const &l)
{
  return s << "λ" << l.s << (l.done() ? "=" : "");
}

O &operator<<(O &s, Λ &l)
{
  s << "Λ λ=" << l.i() << " r=";
  for (let i : l.rs) s << i << " "; s << std::endl;
  for (let &[k, v] : l.ls) s << "  " << k << "\t" << *v << std::endl;
  return s;
}


}

#include "end.hh"
