#ifndef τη_h
#define τη_h

#include "ctypes.hh"
#include "types.hh"
#include "begin.hh"

namespace τ
{


enum class ηsig : u8
{
  α = 0,
  κ = 1,
  i = 2,
  τ = 3,
  ω = 4,

  max = 4,
};


enum class ηatom : u8
{
  ηfalse   = 0,
  ηtrue    = 1,
  bool_max = 1,
  ηnull    = 2,
  max      = 2,
};


enum class ηtype : u8
{
  sig      = 0,
  n_int    = 1,
  n_float  = 2,
  string   = 3,
  atom     = 4,
  η        = 5,
  name     = 6,
  res7     = 7,   // reserved
  int8s    = 8,
  int16s   = 9,
  int32s   = 10,
  int64s   = 11,
  float32s = 12,
  float64s = 13,
  res14    = 14,  // reserved
  res15    = 15,  // reserved
  max      = 15,
};


O &operator<<(O&, ηsig);
O &operator<<(O&, ηatom);
O &operator<<(O&, ηtype);


// η input: read from fixed location in memory
struct ηi final
{
  ηi(u8c *a, uN l) : a_(a), l_(l) { decode_cb(); }

  ηtype   t() const { return Sc<ηtype>(*a_ >> 4); }
  u8c *data() const { return a_ + c_; }
  uN   size() const { return s_; }

  bool has_next() const { return s_ + c_ < l_; }
  ηi   next()     const { return {data() + size(), l_ - (s_ + c_)}; }


  bool operator==(Stc  &s_) const { return is_s() && s() == s_; }
  bool operator==(Stvc &s_) const { return is_s() && s() == s_; }
  bool operator==(bool b_)  const { return is_b() && b() == b_; }
  bool operator==(i64  i_)  const { return is_i() && i() == i_; }


  // FIXME: this should return a lens
  u8c *operator[](uN i) const
    { ηi r = *this;
      while (i && has_next()) r = r.next(), --i;
      if (i) return nullptr;
      return r.a_; }


  bool is_sig()  const { return t() == ηtype::sig; }
  bool is_i()    const { return t() == ηtype::n_int; }
  bool is_f()    const { return t() == ηtype::n_float; }
  bool is_s()    const { return t() == ηtype::string; }
  bool is_a()    const { return t() == ηtype::atom; }
  bool is_b()    const { return is_a() && a() <= ηatom::bool_max; }
  bool is_η()    const { return t() == ηtype::η; }

  bool is_i8s()  const { return t() == ηtype::int8s; }
  bool is_i16s() const { return t() == ηtype::int16s; }
  bool is_i32s() const { return t() == ηtype::int32s; }
  bool is_i64s() const { return t() == ηtype::int64s; }

  bool is_f32s() const { return t() == ηtype::float32s; }
  bool is_f64s() const { return t() == ηtype::float64s; }


  ηi η() const { return {data(), size()}; }


  ηsig sig() const
    { A(is_sig(), "sig() on non-sig " << t());
      A(size() == 1, "invalid sig size: " << size());
      let x = *data();
      A(x <= Sc<u8>(ηsig::max), "ηsig overflow: " << x);
      return Sc<ηsig>(x); }


  u64 u() const { return i(); }
  i64 i() const
    { A(is_i(), "i() on non-int " << t());
      switch (size())
      {
      case 1: return *Rc<i8bc*> (data());
      case 2: return *Rc<i16bc*>(data());
      case 4: return *Rc<i32bc*>(data());
      case 8: return *Rc<i64bc*>(data());
        TA(0, "η invalid int size: " << size());
      } }

  f64 f() const
    { A(is_f(), "f() on non-float " << t());
      switch (size())
      {
      case 4: return *Rc<f32bc*>(data());
      case 8: return *Rc<f64bc*>(data());
        TA(0, "η invalid float size: " << size());
      } }

  Stv s() const
    { A(is_s(), "s() on non-string " << t());
      return {Rc<chc*>(data()), size()}; }

  ηatom a() const
    { A(is_a(), "a() on non-atom " << t());
      A(size() == 1, "invalid atom size: " << size());
      return Sc<ηatom>(*data()); }

  bool b() const
    { A(is_b(), "b() on non-bool " << t());
      return a() == ηatom::ηtrue; }

  Sn<i8bc> i8s() const
    { A(is_i8s(), "i8s() on " << t());
      return {Rc<i8bc*>(data()), size()}; }

  Sn<i16bc> i16s() const
    { A(is_i16s(), "i16s() on " << t());
      A(!(size() & 1), "i16s invalid size: " << size());
      return {Rc<i16bc*>(data()), size() >> 1}; }

  Sn<i32bc> i32s() const
    { A(is_i32s(), "i32s() on " << t());
      A(!(size() & 3), "i32s invalid size: " << size());
      return {Rc<i32bc*>(data()), size() >> 1}; }

  Sn<i64bc> i64s() const
    { A(is_i64s(), "i64s() on " << t());
      A(!(size() & 7), "i64s invalid size: " << size());
      return {Rc<i64bc*>(data()), size() >> 1}; }

  Sn<f32bc> f32s() const
    { A(is_f32s(), "f32s() on " << t());
      A(!(size() & 3), "f32s invalid size: " << size());
      return {Rc<f32bc*>(data()), size() >> 1}; }

  Sn<f64bc> f64s() const
    { A(is_f64s(), "f64s() on " << t());
      A(!(size() & 7), "f64s invalid size: " << size());
      return {Rc<f64bc*>(data()), size() >> 1}; }


private:
  u8c *a_;  // control byte location
  uN   l_;  // outer buffer length
  uN   s_;  // immediate size
  u8   c_;  // control length (byte + size); a_ + c_ == data

  void decode_cb()
    { A(l_ > 0, "η control byte OOB: l = 0");
      let sn = *a_ & 0x0f;
      if      (sn <= 0x0c) c_ = 1, s_ = sn;
      else if (sn == 0x0d)
      { A(l_ >= 3, "η control size OOB: l = " << l_ << ", which is < 3");
        c_ = 2, s_ = 13 + a_[1]; }
      else if (sn == 0x0e)
      { A(l_ >= 3, "η control size OOB: l = " << l_ << ", which is < 3");
        c_ = 3, s_ = 269 + (Sc<u16>(a_[0]) << 8 | a_[1]); }
      else
      { A(l_ >= 5, "η control size OOB: l = " << l_ << ", which is < 5");
        c_ = 5;
        s_ = 65805 + (  Sc<u32>(a_[0]) << 24
                      | Sc<u32>(a_[1]) << 16
                      | Sc<u32>(a_[2]) << 8
                      | a_[3]); }
      A(s_ + c_ <= l_,
        "η size overflows container: s_ = " << s_ << ", c_ = " << c_ << ", l_ = " << l_); }
};


}

#include "end.hh"


#endif
