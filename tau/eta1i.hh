#ifndef τη1i_h
#define τη1i_h


#include "eta-types.hh"
#include "eta0i.hh"

#include "begin.hh"

namespace τ
{


// NOTE: this whole module is low-level stuff for η
// TODO: move all of this into eta.hh; no sense in duplicating it


struct η1i
{
  η0i const *i_;
  η1i(η0i const *i) : i_(i) {}

  η0t type() const { return i_->type(); }

  bool is_τ() const;
};


struct η1si : public η1i  // signal input
{
  operator η1sig() const { return Sc<η1sig>(R<u8>(i_->data(), 0)); }
};


inline bool η1i::is_τ() const
{
  return type() == η0t::signal && Sc<η1sig>(η1si{i_}) == η1sig::τ;
}


struct η1pi : public η1i  // primitive input
{
  bool is_u() const { return i_->type() == η0t::uint_be; }
  bool is_s() const { return i_->type() == η0t::int_be; }
  bool is_i() const { return is_u() || is_s(); }
  bool is_f() const { return i_->type() == η0t::float_be; }
  bool is_b() const { return i_->type() == η0t::boolean; }

  operator bool() const { return R<u8>(i_->data(), 0); }

  operator f64() const { return R<f64>(i_->data(), 0); }
  operator u64() const
    { if (is_f()) return Sc<u64>(Sc<f64>(*this));
      let d = i_->data();
      u64 x = 0;
      for (uN j = 0; j < i_->size(); ++j) x = x << 8 | d[j];
      return x; }

  operator i64() const
    { if (is_f()) return Sc<i64>(Sc<f64>(*this));
      let d = i_->data();
      i64 x = *d & 128 ? -1 : 0;
      for (uN j = 0; j < i_->size(); ++j) x = x << 8 | d[j];
      return x; }
};


struct η1ti : public η1i  // tuple-oriented input (also works for sets)
{
  uN         len()     const;
  η0i operator[](uN j) const;

  struct it
  {
    u8c *x;
    it  &operator++()                  { x += η0i(x).osize(); return *this; }
    η0i  operator* ()            const { return η0i{x}; }
    bool operator==(it const &y) const { return x == y.x; }
  };

  it begin() const { return it{i_->data()}; }
  it end()   const { return it{i_->data() + i_->size()}; }
};


O &operator<<(O &s, η1i const &x);
O &operator<<(O &s, η1si const &x);
O &operator<<(O &s, η1pi const &x);
O &operator<<(O &s, η1ti const &x);


}

#include "end.hh"


#endif
