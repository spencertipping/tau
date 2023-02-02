#include "Xi.hh"
#include "begin.hh"

namespace τ
{


O &operator<<(O &s, Ξ const &x)
{
  s << "Ξ" << x.a_.size() << "(";
  bool first = true;
  for (let &[k, _] : x.n_)
  { if (first) first = false;
    else       s << " ";
    s << k; }
  s << ")[";
  first = true;
  for (let &[k, _] : x.x_)
  { if (first) first = false;
    else       s << " ";
    s << k; }
  return s << "]";
}


}

#include "end.hh"
