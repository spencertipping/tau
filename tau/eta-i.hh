#ifndef τηi_h
#define τηi_h

#include "ctypes.hh"
#include "types.hh"
#include "eta-types.hh"

#include "begin.hh"

namespace τ
{


struct ηi;
Tt struct ηauto_;

typedef ηi const ηic;

// NOTE: copy is intentional
O &operator<<(O&, ηi);


enum class ηicb_r : u8
{
  ok,            // ηi can be decoded
  no_ctrl,       // too short for control byte
  short_len1,    // too short for 1-byte length
  short_len2,    // too short for 2-byte length
  short_len4,    // too short for 4-byte length
  int_overflow,  // size + clen overflows int
  size_overflow  // declared size overflows container length
};

O &operator<<(O&, ηicb_r);


// Check to see whether we can decode an ηi from the given location.
// This function is always safe, as long as the input points to valid
// memory.
T<ηicb_r, uN, u8> ηicb(u8c*, uN);


// η input: read from fixed location in memory
struct ηi final
{
  ηi(ηic&) = default;
  ηi()                   : a_(nullptr),  l_(0)              { decode_cb(); }
  ηi(u8c *a, uN l)       : a_(a),        l_(l)              { decode_cb(); }
  explicit ηi(Sn<u8c> s) : a_(s.data()), l_(s.size_bytes()) { decode_cb(); }

  ηi &operator=(ηi const&) = default;
  ηi &operator=(Sn<u8c> const &s) { a_ = s.data(); l_ = s.size_bytes(); decode_cb(); return *this; }

  ηtype       t()    const { A(!empty(), "t() on η empty"); return Sc<ηtype>(*a_ >> 4); }
  u8c     *data()    const { return a_ + c_; }
  uN       size()    const { return s_; }
  u8c    *odata()    const { return a_; }
  uN      osize()    const { return s_ + c_; }
  u8c    *adata()    const { return a_ + s_ + c_; }
  uN      asize()    const { return l_ - osize(); }  // size after this
  u8c    *ldata()    const { return odata(); }       // full data
  uN      lsize()    const { return l_; }            // full size
  Sn<u8c> inner()    const { return {data(),  size()}; }
  Sn<u8c> outer()    const { return {a_,      osize()}; }
  Sn<u8c>   all()    const { return {a_,      lsize()}; }
  Sn<u8c> after()    const { return {adata(), has_next() ? asize() : 0}; }

  bool    has_next() const { return l_ > osize(); }
  ηi      next()     const { return {adata(), has_next() ? asize() : 0}; }

  ηi      one()      const { return {a_, osize()}; }

  ηi &operator++()    { *this = next(); return *this; }
  ηi  operator++(int) { let r = *this; ++*this; return r; }

  bool empty()             const { return l_ == 0; }
  explicit operator bool() const { return !empty(); }

  Txs operator T<Xs...>() const { return ηauto_<T<Xs...>>::v(*this); }

  uN len() const
    { ηi i = *this;
      uN r = 0;
      while (i) ++r, ++i;
      return r; }

  ηi sub(uN start, uN n) const
    { ηi i1 = *this; while (i1 && start--) ++i1;
      ηi i2 = i1;    while (i2 && n--) ++i2;
      return ηi{i1.a_, i2.a_ >= i1.a_ ? uN(i2.a_ - i1.a_) : i1.l_}; }

  ηi last() const
    { ηi i = *this;
      while (i.has_next()) ++i;
      return i; }

  // Type of each element within this η record, up to the first 15 (used for
  // fast dispatch)
  ηtl ts() const;


  struct it
  {
    Sn<u8c> x;
    bool operator==(it const &y) const { return x.empty() == y.x.empty(); }
    it  &operator++()                  { x = ηi(x).after(); return *this; }
    ηi   operator* ()            const { return ηi(x); }
  };

  // NOTE: iteration applies to this and subsequent items in the stream
  it begin() const { return it{{a_, l_}}; }
  it end()   const { return it{{a_ + l_, 0}}; }

  Txs citr<ηi, it, T<Xs...>> every() const { return {*this}; }


  bool operator==(Stc  &s_) const { return is_s() && s() == s_; }
  bool operator==(Stvc &s_) const { return is_s() && s() == s_; }
  bool operator==(bool  b_) const { return is_b() && b() == b_; }
  bool operator==(i64   i_) const { return is_i() && i() == i_; }
  bool operator==(ηic  &b_) const { return (*this <=> b_) == PO::equivalent; }

  bool is_n(Stc  &n_) const { return is_n() && n() == n_; }
  bool is_n(Stvc &n_) const { return is_n() && n() == n_; }


  PO operator<=>(ηi const &x) const;


  ηi operator[](uN)           const;
  ηi operator[](ηname const&) const;


  u64  hash()   const;  // hash for data structures
  h256 sha256() const;  // hash for collision resistance


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

  bool is_α() const { return is_sig() && sig() == ηsig::α; }
  bool is_ι() const { return is_sig() && sig() == ηsig::ι; }
  bool is_κ() const { return is_sig() && sig() == ηsig::κ; }
  bool is_τ() const { return is_sig() && sig() == ηsig::τ; }
  bool is_ω() const { return is_sig() && sig() == ηsig::ω; }
  bool is_Ω() const { return is_sig() && sig() == ηsig::Ω; }


  ηi η() const { A(is_η(), "ηi::η() on " << t()); return {data(), size()}; }


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


  i64 ci() const
    { switch (t())
      {
      case ηtype::n_int:   return i();
      case ηtype::n_float: return i64(f());
      case ηtype::atom:    return a() == ηatom::ηtrue;
      case ηtype::string:  return std::stoll(St{s()});
      default: A(0, "η::ci(" << t() << "=" << *this << ")"); return 0;
      } }

  f64 cf() const
    { switch (t())
      {
      case ηtype::n_int:   return f64(i());
      case ηtype::n_float: return f();
      case ηtype::string:  return std::stod(St{s()});
      default: A(0, "η::cf(" << t() << "=" << *this << ")"); return 0;
      } }

  bool cb() const
    { switch (t())
      {
      case ηtype::n_int:   return i();
      case ηtype::n_float: return f() != 0.0 && !std::isnan(f());
      case ηtype::atom:    return b();
      case ηtype::string:  return !s().empty();
      default: A(0, "η::cb(" << t() << "=" << *this << ")"); return 0;
      } }

  St cs() const
    { switch (t())
      {
      case ηtype::n_int:   return (Ss{} << i()).str();
      case ηtype::n_float: return (Ss{} << f()).str();
      case ηtype::atom:    return (Ss{} << a()).str();
      case ηtype::name:    return St{n()};
      case ηtype::string:  return St{s()};
      default: A(0, "η::cs(" << t() << "=" << *this << ")"); return 0;
      } }


private:
  u8c *a_;  // control byte location
  uN   l_;  // outer buffer length
  uN   s_;  // immediate size
  u8   c_;  // control length (byte + size); a_ + c_ == data

  void decode_cb()
    { let [r, s, c] = ηicb(a_, l_);
      let r0 = r;  // NOTE: this fixes a reference capture in the A() lambda
      A(r == ηicb_r::ok || r == ηicb_r::no_ctrl,
        "ηi bounds error: " << r0 << " at " << (void*)a_ << "+" << l_);

      // Segfault if we try to access data in an empty η
      if (r == ηicb_r::no_ctrl) a_ = nullptr;
      s_ = s;
      c_ = c; }
};


}


namespace std
{

Tn struct hash<τ::ηi>
{
  size_t operator()(τ::ηic &x) const { return x.hash(); }
};

}


#include "end.hh"


#endif
