#ifndef τηi_h
#define τηi_h

#include "ctypes.hh"
#include "types.hh"
#include "eta-types.hh"

#include "begin.hh"

namespace τ
{


// η input: read from fixed location in memory
struct ηi final
{
  ηi(u8c *a, uN l) : a_(a),        l_(l)              { decode_cb(); }
  ηi(Sn<u8c> s)    : a_(s.data()), l_(s.size_bytes()) { decode_cb(); }

  ηtype    t() const { return Sc<ηtype>(*a_ >> 4); }
  u8c  *data() const { return a_ + c_; }
  uN    size() const { return s_; }
  u8c *odata() const { return a_; }
  uN   osize() const { return s_ + c_; }
  uN   asize() const { return l_ - osize(); }  // size after this

  bool    has_next() const { return asize(); }
  ηi      next()     const { return {data() + size(), asize()}; }
  Sn<u8c> snext()    const { return {data() + size(), has_next() ? asize() : 0}; }


  bool operator==(Stc  &s_) const { return is_s() && s() == s_; }
  bool operator==(Stvc &s_) const { return is_s() && s() == s_; }
  bool operator==(bool b_)  const { return is_b() && b() == b_; }
  bool operator==(i64  i_)  const { return is_i() && i() == i_; }

  bool is_n(Stc  &n_) const { return is_n() && n() == n_; }
  bool is_n(Stvc &n_) const { return is_n() && n() == n_; }


  Sn<u8c> operator[](uN i) const
    { ηi r = *this;
      while (i && has_next()) r = r.next(), --i;
      return {r.a_, i ? 0 : r.l_}; }

  Sn<u8c> operator[](Stc &k) const
    { ηi r = *this;
      for (; r.has_next(); r = r.next())
        if (r.is_n(k)) return r.snext();
      return {r.a_, 0}; }

  Sn<u8c> operator[](Stvc &k) const
    { ηi r = *this;
      for (; r.has_next(); r = r.next())
        if (r.is_n(k)) return r.snext();
      return {r.a_, 0}; }


  bool is_sig()  const { return t() == ηtype::sig; }
  bool is_i()    const { return t() == ηtype::n_int; }
  bool is_f()    const { return t() == ηtype::n_float; }
  bool is_s()    const { return t() == ηtype::string; }
  bool is_a()    const { return t() == ηtype::atom; }
  bool is_n()    const { return t() == ηtype::name; }
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

  Stv n() const
    { A(is_n(), "n() on non-name " << t());
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
      return {Rc<i32bc*>(data()), size() >> 2}; }

  Sn<i64bc> i64s() const
    { A(is_i64s(), "i64s() on " << t());
      A(!(size() & 7), "i64s invalid size: " << size());
      return {Rc<i64bc*>(data()), size() >> 3}; }

  Sn<f32bc> f32s() const
    { A(is_f32s(), "f32s() on " << t());
      A(!(size() & 3), "f32s invalid size: " << size());
      return {Rc<f32bc*>(data()), size() >> 2}; }

  Sn<f64bc> f64s() const
    { A(is_f64s(), "f64s() on " << t());
      A(!(size() & 7), "f64s invalid size: " << size());
      return {Rc<f64bc*>(data()), size() >> 3}; }


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
      A(s_ + c_ > s_, "η size overflow");
      A(s_ + c_ <= l_,
        "η size overflows container: s_ = " << s_ << ", c_ = " << c_ << ", l_ = " << l_); }
};


O &operator<<(O&, ηi const&);


}

#include "end.hh"


#endif
