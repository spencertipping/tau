#include "psi.hh"
#include "begin.hh"

namespace τ
{


static iN ψs_ = 0;

void ψc_(ψ_ *q) { ++ψs_; }
void ψx_(ψ_ *q) { A(--ψs_ >= 0, "ψs underflow"); }
uN   ψn()       { return ψs_; }


void ψ_::destroy()
{
  Λ &l = t_.l();
  for (let  x      : ls_) if (l.e(x) && x != l.i()) l.x(x);
  for (let &[p, _] : cs_)                           t_.unbind(p);
  for (let &f      : df_)                           f(*this);
  ls_.clear();
  cs_.clear();
  df_.clear();
}


ψ_ &ψ_::f(λf &&f)
{
  Λ &l = t_.l();
  for (auto i = ls_.begin(); i != ls_.end();)
    if (!l.e(*i)) i = ls_.erase(i);
    else          ++i;
  ls_.insert(l.c(std::move(f)));
  return *this;
}


O &operator<<(O &s, ψ const &q)
{
  return s << q.name();
}


}

#include "end.hh"
