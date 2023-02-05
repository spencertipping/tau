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

  bool is_τ() const;
};


struct η1si : public η1i  // signal input
{
  operator η0sig() const { return Sc<η0sig>(R<u8>(i.data(), 0)); }
};


inline bool η1i::is_τ() const
{
  return type() == η0t::signal && Sc<η0sig>(η1si{i}) == η0sig::τ;
}


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
      A(0, "η₁ti[] OOB");
      return nullptr; }

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


O &operator<<(O &s, η1i const &x);
O &operator<<(O &s, η1si const &x);
O &operator<<(O &s, η1pi const &x);
O &operator<<(O &s, η1ti const &x);


}

#include "end.hh"


#endif
