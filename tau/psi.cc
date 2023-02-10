#include "psi.hh"
#include "begin.hh"

namespace τ
{


static iN ψs_ = 0;

void ψc_(ψ *q) { ++ψs_; }
void ψx_(ψ *q) { A(--ψs_ >= 0, "ψs underflow"); }
uN ψn()        { return ψs_; }


void ψ::destroy()
{
  Λ &l = t_.l();
  for (let  x : ls) if (l.e(x) && x != l.i()) l.x(x);
  for (let &f : df)                           f(*this);
  ls.clear();
  df.clear();
}


λi ψ::λc(λf &&f)
{
  Λ &l = t_.l();
  for (auto i = ls.begin(); i != ls.end();)
    if (!l.e(*i)) i = ls.erase(i);
    else          ++i;
  return l.c(std::move(f));
}


O &operator<<(O &s, ψ const &q)
{
  return s << q.name();
}


}

#include "end.hh"
