#include "eta1i.hh"
#include "begin.hh"

namespace τ
{


O &operator<<(O &s, η1pi const &x)
{
  if      (x.is_f()) return s << Sc<f64>(x);
  else if (x.is_u()) return s << Sc<u64>(x);
  else if (x.is_s()) return s << Sc<i64>(x);
  else if (x.is_b()) return s << (x ? "t" : "f");
  else               return s << "η₁ni NA";
}

O &operator<<(O &s, η1ti const &x)
{
  s << "(";
  bool first = true;
  for (let y : x)
  { if (first) first = false;
    else       s << ",";
    s << η1i{y}; }
  return s << ")";
}

O &operator<<(O &s, η1i const &x)
{
  if      (η0tc[x.type()]) return s << η1ti{x.i};
  else if (η0tp[x.type()]) return s << η1pi{x.i};
  else if (η0tb[x.type()]) return s << "\"" << Stv{Rc<chc*>(x.i.data()), x.i.size()} << "\"";
  else                     return s << x.i;
}


}

#include "end.hh"
