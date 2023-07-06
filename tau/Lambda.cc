#include "Lambda.hh"
#include "begin.hh"

namespace τ
{


void Λ::clear()
{
  if (fin) return;
  A(!i(), "Λ::clear from non-main thread");
  uN tries = 0;
  while (!ls.empty())
  {
    for (auto &[k, v] : ls) x(k);
    step();  // give all λs a chance to clear themselves
    A(++tries < 100,
      "Λ::clear: " << ls.size() << " λ(s) still exist");
    if (tries > 2)
      std::cerr << (Ss{} << getpid() << " warning: Λ::clear: "
                    << ls.size() << " λ(s) still exist after " << tries << " iterations\n").str();
  }
}


λi Λ::c(λf &&f)
{
  if (fin) return 0;
  let i = ιi(ni, ls);
  ls[i].reset(new Λt([f=mo(f)]()
    { try           { f(); }
      catch (Λx_)   {}
      catch (Stc e)
      { std::cerr << (Ss{} << getpid() << " Λ uncaught: " << e << "; UNIX PROCESS IS EXITING\n").str();
        std::exit(1); } }));
  r(i, λs::R);
  return i;
}

Λ &Λ::x(λi i)
{
  if (fin) return *this;
  if (i == ri) λx();
  else         r(i, λs::X);
  return *this;
}


Λ &Λ::y(λs s)
{
  if (fin) return *this;
  A(i(), "Λ::y from main thread");
  r(ri, s);
  λy();

  // Unwind the λ stack to deallocate its resources, leading to it being
  // destroyed
  if (ls.at(ri)->s == λs::X) throw λx_;

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
  ++cs_;
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

Λ &Λ::step()
{
  for (λi t; (t = (*this)()); *this << t);
  for (let &[i, s] : ls) if (s->s == λs::Y) r(i, λs::R);
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
