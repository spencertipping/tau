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


// η input: read from fixed location in memory
struct ηi final
{
  ηi(ηic&) = default;
  ηi()                   : a_(nullptr),  l_(0)              { decode_cb(); }
  ηi(u8c *a, uN l)       : a_(a),        l_(l)              { decode_cb(); }
  explicit ηi(Sn<u8c> s) : a_(s.data()), l_(s.size_bytes()) { decode_cb(); }

  ηi &operator=(ηi const&) = default;
  ηi &operator=(Sn<u8c> const &s) { a_ = s.data(); l_ = s.size_bytes(); decode_cb(); return *this; }

  // Nomenclature:
  //   data()/size():  the payload of this ηi's first value (no control or size prefix)
  //  odata()/osize(): the outside of this ηi's first value
  //  adata()/asize(): the outside of the η value after this one (next in stream)
  //  ldata()/lsize(): the outside of the entire η stream

  Sn<u8c> inner()    const { return {data(), size()}; }
  u8c     *data()    const { return a_ + c_; }
  uN       size()    const { return s_; }

  Sn<u8c> outer()    const { return {odata(), osize()}; }
  u8c    *odata()    const { return a_; }
  uN      osize()    const { return s_ + c_; }

  Sn<u8c> after()    const { return {adata(), has_next() ? asize() : 0}; }
  u8c    *adata()    const { return a_ + s_ + c_; }
  uN      asize()    const { return l_ - osize(); }

  Sn<u8c>   all()    const { return {a_, lsize()}; }
  u8c    *ldata()    const { return odata(); }
  uN      lsize()    const { return l_; }

  ηtype   t()        const { A(!empty(), "t() on η empty"); return Sc<ηtype>(*a_ >> 4); }
  bool    has_next() const { return l_ > osize(); }
  ηi      next()     const { return ηi{after()}; }

  ηi      one()      const { return {a_, osize()}; }

  ηi &operator++()    { *this = next(); return *this; }
  ηi  operator++(int) { let r = *this; ++*this; return r; }

  bool empty()             const { return l_ == 0; }
  explicit operator bool() const { return !empty(); }

  Txs operator T<Xs...>() const { return ηauto_<T<Xs...>>::v(*this); }
  Txxs auto          as() const
    { if constexpr (sizeof...(Xs) == 0) return ηauto_<X>::v(*this);
      else                              return ηauto_<T<X, Xs...>>::v(*this); }


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


  // k/v iteration
  struct it2
  {
    Sn<u8c> x;
    bool      operator==(it2 const &y) const { return x.empty() == y.x.empty(); }
    it2      &operator++()                   { x = ηi(x).next().after(); return *this; }
    P<ηi, ηi> operator*()              const { let i = ηi(x); return mp(i, i.next()); }
  };

  struct it2_
  {
    Sn<u8c> b;
    Sn<u8c> e;
    it2 begin() const { return {b}; }
    it2 end()   const { return {e}; }
  };

  it2_ kvs() const { return {begin().x, end().x}; }


  // every<X>() unpacks a single value into a non-tuple
  // every<X, Y, ...>() unpacks multiple values from a sub-η
  template<class X> auto every() const
    { return fitr(*this, [](ηic &x) { return ηauto_<X>::v(x); }); }

  template<class X, class Y, class... Xs> auto every() const
    { return fitr(*this, [](ηic &x) { return ηauto_<T<X, Y, Xs...>>::v(x.η()); }); }


  bool operator==(Stc  &s_) const { return is_s() && s() == s_; }
  bool operator==(Stvc &s_) const { return is_s() && s() == s_; }
  bool operator==(bool  b_) const { return is_b() && b() == b_; }
  bool operator==(i64   i_) const { return is_i() && i() == i_; }
  bool operator==(ηic  &b_) const { return (*this <=> b_) == PO::equivalent; }

  bool is_n(Stc &n_) const { return is_n() && n().x == n_; }


  PO operator<=> (ηic &x) const;  // logical ordering (3.0 < 4.0 etc)
  SO fast_compare(ηic &x) const   // NOTE: not compatible with <=>
    { if (let c = lsize() <=> x.lsize(); c != SO::equal) return c;
      let c = memcmp(ldata(), x.ldata(), lsize());
      return c < 0 ? SO::less : c > 0 ? SO::greater : SO::equal; }


  ηi operator[](uN)           const;
  ηi operator[](ηname const&) const;


  u64  hash()   const;  // hash for data structures
  h256 sha256() const;  // hash for collision resistance


  bool is_sig()  const { return !empty() && t() == ηtype::sig; }
  bool is_i()    const { return !empty() && t() == ηtype::n_int; }
  bool is_f()    const { return !empty() && t() == ηtype::n_float; }
  bool is_s()    const { return !empty() && t() == ηtype::string; }
  bool is_a()    const { return !empty() && t() == ηtype::atom; }
  bool is_n()    const { return !empty() && t() == ηtype::name; }
  bool is_b()    const { return !empty() && is_a() && a() <= ηatom::bool_max; }
  bool is_η()    const { return !empty() && t() == ηtype::η; }
  bool is_bin()  const { return !empty() && t() == ηtype::binary; }
  bool is_e()    const { return !empty() && t() == ηtype::ext; }

  bool is_i8s()  const { return !empty() && t() == ηtype::int8s; }
  bool is_i16s() const { return !empty() && t() == ηtype::int16s; }
  bool is_i32s() const { return !empty() && t() == ηtype::int32s; }
  bool is_i64s() const { return !empty() && t() == ηtype::int64s; }
  bool is_f32s() const { return !empty() && t() == ηtype::float32s; }
  bool is_f64s() const { return !empty() && t() == ηtype::float64s; }

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
      A(x <= u8(ηsig::max), "ηsig overflow: " << x);
      return ηsig(x); }


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

  ηname n() const
    { A(is_n(), "n() on non-name " << t());
      return {St{Rc<chc*>(data()), size()}}; }

  Stv bin() const
    { A(is_bin(), "bin() on non-binary " << t());
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
      case ηtype::name:    return n().x;
      case ηtype::string:  return St{s()};
      case ηtype::binary:  return St{bin()};
      default: A(0, "η::cs(" << t() << "=" << *this << ")"); return {};
      } }

  ηbin cbin() const
    { switch (t())
      {
      case ηtype::string: return {St{s()}};
      case ηtype::binary: return {St{bin()}};
      case ηtype::int8s:  return {St{Rc<chc*>(i8s().data()), i8s().size()}};
      default: A(0, "η::cbin(" << t() << "=" << *this << ")"); return {};
      } }


  St inspect() const;


private:
  u8c *a_;  // control byte location
  uN   l_;  // outer buffer length
  uN   s_;  // immediate size
  u8   c_;  // control length (byte + size); a_ + c_ == data

  void decode_cb();
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
