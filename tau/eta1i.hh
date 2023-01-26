#ifndef τη1i_h
#define τη1i_h


#include "eta-types.hh"
#include "eta0i.hh"

#include "begin.hh"

namespace τ
{


struct η1i
{
  η0i i;
  η1i(η0i const &i_) : i(i_.odata())    {}
  η1i(η0i &&i_)      : i(std::move(i_)) {}
  η1i(u8c *a)        : i(a)             {}

  η0t type() const { return i.type(); }
};


struct η1pi : public η1i  // primitive input
{
  bool is_u() const { return i.type() == η0t::uint_be; }
  bool is_s() const { return i.type() == η0t::int_be; }
  bool is_i() const { return is_u() || is_s(); }
  bool is_f() const { return i.type() == η0t::float_be; }
  bool is_b() const { return i.type() == η0t::boolean; }

  operator bool() const { return R<u8>(i.data(), 0); }

  operator f64() const { return R<f64>(i.data(), 0); }
  operator u64() const
    { if (is_f()) return Sc<u64>(Sc<f64>(*this));
      let d = i.data();
      u64 x = 0;
      for (uN j = 0; j < i.size(); ++j) x = x << 8 | d[j];
      return x; }

  operator i64() const
    { if (is_f()) return Sc<i64>(Sc<f64>(*this));
      let d = i.data();
      i64 x = *d & 128 ? -1 : 0;
      for (uN j = 0; j < i.size(); ++j) x = x << 8 | d[j];
      return x; }
};


struct η1ti : public η1i  // tuple-oriented input (also works for sets)
{
  uN len() const
    { uN l = 0;
      for (u8c *x = i.data();
           x < i.data() + i.size();
           x += η0i(x).osize())
        ++l;
      return l; }

  η0i operator[](uN j) const
    { for (u8c *x = i.data();
           x < i.data() + i.size();
           x += η0i(x).osize())
        if (!j--) return x;
      A(0, "η₁ti[] OOB"); }

  struct it
  {
    u8c *x;
    it  &operator++()                  { x += η0i(x).osize(); return *this; }
    η0i  operator* ()            const { return η0i{x}; }
    bool operator==(it const &y) const { return x == y.x; }
  };

  it begin() const { return it{i.data()}; }
  it end()   const { return it{i.data() + i.size()}; }
};


#if τdebug_iostream
O &operator<<(O &s, η1i const &x);

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
#endif


}

#include "end.hh"


#endif
