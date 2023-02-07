#include "eta1i.hh"
#include "begin.hh"

namespace τ
{


uN η1ti::len() const
{
  uN l = 0;
  for (u8c *x = i_->data();
       x < i_->data() + i_->size();
       x += η0i(x).osize())
    ++l;
  return l;
}


η0i η1ti::operator[](uN i) const
{
  for (u8c *x = i_->data();
       x < i_->data() + i_->size();
       x += η0i(x).osize())
    if (!i--) return x;
  A(0, "η₁ti[] OOB");
  return nullptr;
}


O &operator<<(O &s, η1si const &x)
{
  return s << Sc<η1sig>(x);
}

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
    s << η1i{&y}; }
  return s << ")";
}

O &operator<<(O &s, η1i const &x)
{
  if      (η1tc[x.type()]) return s << η1ti{x.i_};
  else if (η1tp[x.type()]) return s << η1pi{x.i_};
  else if (η1tb[x.type()]) return s << "\"" << Stv{Rc<chc*>(x.i_->data()), x.i_->size()} << "\"";
  else                     return s << x.i_;
}


}

#include "end.hh"
