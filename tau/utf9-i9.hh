#ifndef τutf9_i9_h
#define τutf9_i9_h


#include <algorithm>
#include <cstring>
#include <memory>


#include "arch.hh"
#include "debug.hh"
#include "numerics.hh"
#include "types.hh"
#include "zeta.hh"

#include "utf9-types.hh"

#include "begin.hh"

namespace τ
{


static u8 i9_statics[] =
{
  u9t::b      | u9s::v8, 1,  0 | 1,                               // offset 0 = false
  u9t::b      | u9s::v8, 1, 16 | 1,                               // offset 3 = true
  u9t::symbol | u9s::v8, 1, 0,                                    // offset 6 = null symbol
  u9t::none   | u9s::v8, 1, Sc<u8>(u9_none::key_lookup_missing),  // offset 9 = no key
  u9t::none   | u9s::v8, 1, Sc<u8>(u9_none::tuple_bounds_error),  // offset 12 = tuple OOB
  u9t::none   | u9s::v8, 1, Sc<u8>(u9_none::invalid_value),       // offset 15 = decoding error
  u9t::none   | u9s::v8, 1, Sc<u8>(u9_none::generic),             // offset 18 = generic error
};


struct i9;
inline i9 i9_false();
inline i9 i9_true();
inline i9 i9_null();
inline i9 i9_none();
inline i9 i9_no_key();
inline i9 i9_tuple_bounds();


enum class i9v : u8
{
  valid         = 0,
  osize_overrun = 1,
  osize_2       = 2,
  osize_sb      = 3,
  undersized    = 4,
  bad_element   = 5,
  bad_collsize  = 6,
};


#if τdebug_iostream
O &operator<<(O&, i9 const&);
O &operator<<(O&, i9v);
#endif


struct i9
{
  static inline uN   size_of(ζp a) { return u9rs(a, 0); }
  static inline i9   cast   (ζp a) { return i9{a}; }
  static inline void free   (ζp a) { i9 i{a}; if (i.is_heapref()) i.free(); }


  struct it
  {
    ζp a;
    operator     ζp()            const { return a; }
    i9   operator* ()            const { return i9{a}; }
    bool operator==(it const &x) const { return a == x.a; }
    it   operator++()                  { a += i9{a}.osize(); return *this; }
  };


  struct kit  // key iterator
  {
    it i;
    i9   operator* ()             const { return *i; }
    bool operator==(kit const &x) const { return  i == x.i; }
    kit  operator++()                   { ++i; ++i; return *this; }
  };

  struct ks
  {
    ζp a;
    kit begin() const { return kit{it{a + u9sb(u9ts_s(R<u8>(a, 0)))}}; }
    kit end()   const { return kit{it{a + u9rs(a, 0)}}; }
  };


  struct fit  // flagged iterator
  {
    sletc u9c = u9tm{u9t::index, u9t::map, u9t::set, u9t::tuple};

    it       i;
    it const e;

    i9   operator* ()             const { return *i; }
    bool operator==(fit const &x) const { return  i == x.i; }
    fit &operator++()
      { while (1)
        { if (i == e) return *this;
          let j = *i;
          if (j.flagged())
            if (u9c[j.type()]) i.a = j.first();
            else               i.a = j.next();
          else ++i;
          if ((*i).flagged()) return *this; } }
  };

  struct fs
  {
    ζp a;
    fit begin() const { return a ? fit{it{a},                  it{a + i9::size_of(a)}} : fit{{nullptr}, {nullptr}}; }
    fit end()   const { return a ? fit{it{a + i9::size_of(a)}, it{a + i9::size_of(a)}} : fit{{nullptr}, {nullptr}}; }
  };


  ζp a;
  i9() : a(nullptr) {}
  i9(ζp a_) : a(a_) {}


  // TODO: create names for UTF9 states and formalize definitions
  bool  exists() const { return a && a != ζωp; }
  bool    real() const { return exists() && type() != u9t::stream; }
  operator  ζp() const { return a; }


  uf8  code()    const { return R<u8>(a, 0); }
  bool flagged() const { return a ? u9ts_f(code()) : false; }
  u9s  stype()   const { return a ? u9ts_s(code()) : u9s::v8; }
  u9t  type()    const { return a ? u9ts_t(code()) : u9t::none; }
  i9   next()    const { return a + osize(); }
  i9   first()   const { return a ? data() : nullptr; }
  i9   second()  const { return first().next(); }
  ks   keys()    const { return ks{a}; }
  fs   flags()   const { return flagged() ? fs{a} : fs{0}; }
  uN   osize()   const { return a ? u9rs(a, 0) : 0; }

  // NOTE: iteration supported only for collections of UTF9 objects, and
  // indexes require .second() if you want to traverse elements
  it   begin()   const { return it{data()}; }
  it   end()     const { return it{next()}; }

  inline bool b() const;


  // NOTE: inner, "logical" size, not outer size; that way these methods
  // have STL-style meanings
  ζp   data()    const { return a + u9sb(stype()); }
  uN   size()    const { return end() - begin(); }

  // NOTE: returns 0 if this type cannot be vectorized
  uN   vn()      const
    { if (type() == u9t::b)
      { let r = R<u8>(data(), 0) & 7;
        return r <= 4 ? size() - 1 << 3 | r : size() - 2 << 3 | r; }
      else
        return size() >> u9logsizeof(type()); }


  template<class T> operator u9_scoped<u9_π,     T>() const { u9tm{u9t::pi}   (type()); return R<u9_scoped<u9_π,     T>>(data(), 0); }
  template<class T> operator u9_scoped<u9_Φ,     T>() const { u9tm{u9t::phi}  (type()); return R<u9_scoped<u9_Φ,     T>>(data(), 0); }
  template<class T> operator u9_scoped<u9_host,  T>() const { u9tm{u9t::host} (type()); return R<u9_scoped<u9_host,  T>>(data(), 0); }
  template<class T> operator u9_scoped<u9_build, T>() const { u9tm{u9t::build}(type()); return R<u9_scoped<u9_build, T>>(data(), 0); }

  operator u9_π()     const { u9tm{u9t::pi}   (type()); return Sc<u9_π>    (R<u9_scoped_t>(data(), 0)); }
  operator u9_Φ()     const { u9tm{u9t::phi}  (type()); return Sc<u9_Φ>    (R<u9_scoped_t>(data(), 0)); }
  operator u9_host()  const { u9tm{u9t::host} (type()); return Sc<u9_host> (R<u9_scoped_t>(data(), 0)); }
  operator u9_build() const { u9tm{u9t::build}(type()); return Sc<u9_build>(R<u9_scoped_t>(data(), 0)); }

  bool is_heapref() const { return type() == u9t::phi && Sc<u9_Φ>(*this) == u9_Φ::heapref; }
  bool is_heappin() const { return type() == u9t::phi && Sc<u9_Φ>(*this) == u9_Φ::heappin; }
  bool is_heap()    const { return is_heapref() || is_heappin(); }

  bool is_π()       const { return type() == u9t::pi; }
  bool is_πref()    const { return is_π() && Sc<u9_π>(*this) == u9_π::ref; }
  ζp   πref()       const { return Sc<u9_scoped<u9_π, ζp>>(*this).x; }
  i9   deref()      const { return is_πref() ? (**this).deref() : *this; }

  bool is_istruct() const { return type() == u9t::build && Sc<u9_build>(*this) == u9_build::istruct; }

  bool is_idx()     const { return type() == u9t::index; }
  bool is_tuple()   const { return type() == u9t::tuple; }
  bool is_set()     const { return type() == u9t::set; }
  bool is_map()     const { return type() == u9t::map; }

  i9 ivec()  const { u9tm{u9t::index}(type()); return first().deref(); }
  i9 icoll() const { u9tm{u9t::index}(type()); return second().deref(); }
  u8 ibits() const { u9tm{u9t::index}(type()); return Sc<u8>(second().next()); }

  i9 as_tuple() const { return is_idx() ? icoll().as_tuple() : is_tuple() ? deref() : i9_none(); }
  i9 as_set()   const { return is_idx() ? icoll().as_set()   : is_set()   ? deref() : i9_none(); }
  i9 as_map()   const { return is_idx() ? icoll().as_map()   : is_map()   ? deref() : i9_none(); }


  template<class T> T* ptr() const { return Rc<T*>(Sc<u9_scoped<u9_Φ, void*>>(*this).x); }


  template<class T>
  requires u9t_hastype<T> && u9t_is<T, u9fixed.m>::v
    explicit operator T() const;

  template<class T>
  operator T*() const
    { A(is_istruct(), "i9 T* on non-struct " << *this);
      A(sizeof(T) == size() - 1, "i9 native T* mismatches bounds; |T|=" << sizeof(T) << ", size()=" << size());
      return Rc<T*>(data() + 1); }


  operator St       () const { return St{Rc<ch*>(data()), size()}; }
  operator Bv       () const { return Bv{data(), size()}; }
  operator u9_symbol() const { u9tm{u9t::symbol}(type()); return u9_symbol{B(data(), size())}; }

  operator u9st() const
    { u9tm{u9t::stream}(type());
      return Sc<u9st>(R<u8>(begin(), 0)); }

  u8 dim() const { return type() == u9t::stream ? R<u8>(begin(), 1) : 0; }

  u64  θ() const { return type() == u9t::stream && Sc<u9st>(*this) == u9st::θ ? R<u64>(begin(), 2) : 0; }
  u64  ι() const { return type() == u9t::stream && Sc<u9st>(*this) == u9st::ι ? R<u64>(begin(), 2) : 0; }
  bool τ() const { return type() == u9t::stream && Sc<u9st>(*this) == u9st::τ; }
  bool α() const { return type() == u9t::stream && Sc<u9st>(*this) == u9st::α; }
  bool ω() const { return type() == u9t::stream && Sc<u9st>(*this) == u9st::ω; }
  bool κ() const { return type() == u9t::stream && Sc<u9st>(*this) == u9st::κ; }


  i9 operator*() const
    { A(is_heap() || is_πref(),
        "i9* requires heap or π, got type " << type() << " = " << *this);
      return is_heap()
           ? Sc<u9_scoped<u9_Φ, ζp>>(*this).x
           : Sc<u9_scoped<u9_π, ζp>>(*this).x; };

  i9 operator++() { a = next(); return *this; }


  i9 &pin()   { if (is_heapref()) W(data(), 0, Sc<u8>(u9_Φ::heappin)); return *this; }
  i9 &unpin() { if (is_heappin()) W(data(), 0, Sc<u8>(u9_Φ::heapref)); return *this; }
  i9 &free()  { if (is_heap()) std::free((**this).a); return *this; }


  uN len() const  // number of contained elements
    { u9tm{u9t::tuple, u9t::set, u9t::map}(type());
      uN r = 0;
      for (let _ : *this) (void)_, ++r;
      return r; }


  sletc ib_search_limit = 16;
  sletc bl_search_limit = 4;

  template<class T>
  uN iv_bsearch(T x, uN l = 0, uN u = -1) const
    { if (vn() < 2) return 0;
      if (u > vn()) u = vn();
      while (l + 2 < u)
      { let m = std::midpoint(l, u) & ~Sc<uN>(1);
        let y = at<T>(m);
        if      (y == x) return m;
        else if (y < x)  l = m;
        else             u = m; }
      return l; }

  template<class T>
  uN iv_isearch(T x) const
    { if (vn() < ib_search_limit) return iv_bsearch(x);
      uN l = 0, u = vn() - 2;
      while (l + ib_search_limit < u)
      { let al = at<T>(l);
        let au = at<T>(u);
        let m  = l + ((x - al) * (u - l)) / (au - al) & ~Sc<uN>(1);
        if (m == l || m == u) return iv_bsearch(x, l, u + 2);
        let y  = at<T>(m);
        if      (y == x) return m;
        else if (y < x)  l = m;
        else             u = m; }
      return iv_bsearch(x, l, u + 2); }


  uN iv_nsearch(uN i) const
    { switch (type())
      {
      case u9t::u8:  return iv_bsearch<u8> (i);
      case u9t::u16: return iv_isearch<u16>(i);
      case u9t::u32: return iv_isearch<u32>(i);
      case u9t::u64: return iv_isearch<u64>(i);
        TA(0, "iv_nsearch invalid ivec type " << type());
      } }

  uN iv_hsearch(u64 h) const
    { switch (type())
      {
      case u9t::u8:  return iv_bsearch<u8> (h >> 56);
      case u9t::u16: return iv_isearch<u16>(h >> 48);
      case u9t::u32: return iv_isearch<u32>(h >> 32);
      case u9t::u64: return iv_isearch<u64>(h);
        TA(0, "iv_hsearch invalid ivec type " << type());
      } }


  // NOTE: can't do vectors here because we don't have enough space
  // to return type information alongside the value; use .at()
  i9 operator[](uN i) const
    { u9tm{u9t::index, u9t::tuple}(type());
      switch (type())
      {
      case u9t::tuple: return tlin(i);
      case u9t::index:
      { let ix = ivec(); if (ix.vn() < bl_search_limit) return icoll().tlin(i);
        let n  = ix.iv_nsearch(i);
        let k  = ix.at<uN>(n);
        return k <= i
             ? icoll().tlin(i - k, ix.at<uN>(n + 1))
             : icoll().tlin(i); }
        TA(0, "i9[uN] requires index or tuple, not " << type())
      } }

  i9 tlin(uN i, uN o = 0) const
    { ζp b = data() + o; while (i--) b += size_of(b); return b; }

  i9 operator[](i9 i) const
    { u9tm{u9t::tuple, u9t::set, u9t::map, u9t::index}(type());
      switch (type())
      {
      case u9t::tuple: return (*this)[Sc<uN>(i)];
      case u9t::set: { for (let x : *this)  if (x == i) return i9_true(); return i9_false(); }
      case u9t::map: { for (let x : keys()) if (x == i) return x.next();  return i9_no_key(); }
      case u9t::index:
      { let ix = ivec();
        let c  = icoll();
        if (ix.vn() < bl_search_limit) return c[i];
        let h = i.h();
        switch (c.type())
        {
        case u9t::tuple: return (*this)[Sc<uN>(i)];
        case u9t::set:
        { let k = ix.iv_hsearch(h);
          for (i9 x = i9{c.data() + ix.at<uN>(k + 1)}, e = x.next();
               x.a < e.a;
               ++x)
          { if (x == i)    return i9_true();
            if (x.h() > h) return i9_false(); }
          return i9_false(); }
        case u9t::map:
        { let k = ix.iv_hsearch(h);
          for (i9 x = i9{c.data() + ix.at<uN>(k + 1)}, e = x.next();
               x.a < e.a;
               ++x, ++x)
          { if (x == i)    return x.next();
            if (x.h() > h) return i9_no_key(); }
          return i9_no_key(); }
          TA(*this, "i9[i9] undefined for indexed " << c.type());
        } }
      TA(*this, "i9[i9] undefined for " << type());
      } }


  u64 h() const
    { let t = type();

      // FIXME: expand the range of hashable types
      u9hashable(t);
      if (t == u9t::symbol) return std::hash<u9_symbol>{}(*this);
      return XXH3_64bits_withSeed(data(), size(), Sc<uN>(t)); }


  bool is_sord() const
    { u9tm{u9t::set}(type());
      u64 h = 0;
      for (let x : *this)
      { let xh = x.h(); if (xh < h) return false; h = xh; }
      return true; }

  bool is_mord() const
    { u9tm{u9t::map}(type());
      u64 h = 0;
      for (let x : keys())
      { let xh = x.h(); if (xh < h) return false; h = xh; }
      return true; }


  bool operator==(uN   x) const { return u9unsigned[type()] && Sc<uN> (*this) == x; }
  bool operator==(i64  x) const { return u9signed[type()]   && Sc<i64>(*this) == x; }
  bool operator==(i32  x) const { return u9signed[type()]   && Sc<i32>(*this) == x; }
  bool operator==(Stc &x) const { return u9strings[type()]  && size() == x.size() && !std::memcmp(data(), x.data(), x.size()); }
  bool operator==(Bc  &x) const { return u9strings[type()]  && size() == x.size() && !std::memcmp(data(), x.data(), x.size()); }

  // FIXME: == should automatically collapse references for flagged containers
  bool operator==(i9   x) const { return x.a == a || type() == x.type() && size() == x.size() && !std::memcmp(data(), x.data(), x.size()); }


  i9v verify(uN s = 0) const  // verify this value with the given external size
    { if (!s) s = osize();
      if (s < 2)                return i9v::osize_2;
      if (s < u9sb(u9ts_s(*a))) return i9v::osize_sb;
      if (s < osize())          return i9v::osize_overrun;
      let e = a + osize();

      // TODO: verify vector sizes

      // TODO: verify index offsets
      if (u9tm{u9t::tuple, u9t::set, u9t::map, u9t::index}[type()])
      { i9 i = first();
        for (; i.a >= first() && i < e; i = i.next())
          if (i.verify(e - i.a) != i9v::valid) return i9v::bad_element;
        if (i.a != e) return i9v::bad_collsize; }
      return i9v::valid; }


  i9 &retype(u9t f, u9t t)
    { A(type() == f, f << " → " << t << " retyping mismatch: "
                       << type() << " ≠ " << f);
      *a = *a & 7 | Sc<u8>(t) << 3;
      return *this; }


  V<i9> &into(V<i9> &xs, uN n = 0) const
    { u9tm{u9t::tuple, u9t::set, u9t::map}(type());
      xs.reserve(len() + n);
      for (let x : *this) xs.push_back(x);
      return xs; }

  M<i9, i9> &into(M<i9, i9> &xs) const;


  // Vector accessors
  template<class T> inline T  at (uN)    const;
  template<class T> inline i9 set(uN, T) const;

  inline bool bvec_at(uN i) const
    { u9tm{u9t::b}(type());
      A(i < vn(), "i9at bool OOB, i = " << i << ", vn = " << vn() << ", sz = " << size());
      let j = i + 4;
      return R<u8>(data(), j >> 3) & (1 << (j & 7)); }

  inline i9 bvec_set(uN i, bool x) const
    { u9tm{u9t::b}(type());
      A(i < vn(), "i9set bool OOB, i = " << i << ", vn = " << vn() << ", sz = " << size());
      let j = i + 4;
      W(data(), j >> 3,
        Sc<u8>(x ? R<u8>(data(), j >> 3) |  (1 << (j & 7))
                 : R<u8>(data(), j >> 3) & ~(1 << (j & 7))));
      return *this; }
};


template<class T>
T i9::at(uN i) const
{
  let t = type();
  if (t == u9t::b) return u9Sc<bool, T>::f(bvec_at(i));

  u9vectors(t);
  let s = u9sizeof(t);
  A((i + 1) * s <= size(),  // important: final byte is checked
    "i9at OOB, i = " << i << ", t = " << t << ", w = " << s << ", sz = " << size());

  switch (t)
  {
  case u9t::i8:  return u9Sc<i8,  T>::f(R<i8> (data(), i * s));
  case u9t::i16: return u9Sc<i16, T>::f(R<i16>(data(), i * s));
  case u9t::i32: return u9Sc<i32, T>::f(R<i32>(data(), i * s));
  case u9t::i64: return u9Sc<i64, T>::f(R<i64>(data(), i * s));
  case u9t::u8:  return u9Sc<u8,  T>::f(R<u8> (data(), i * s));
  case u9t::u16: return u9Sc<u16, T>::f(R<u16>(data(), i * s));
  case u9t::u32: return u9Sc<u32, T>::f(R<u32>(data(), i * s));
  case u9t::u64: return u9Sc<u64, T>::f(R<u64>(data(), i * s));

  case u9t::f32: return u9Sc<f32, T>::f(R<f32>(data(), i * s));
  case u9t::f64: return u9Sc<f64, T>::f(R<f64>(data(), i * s));
  case u9t::c32: return u9Sc<c32, T>::f(R<c32>(data(), i * s));
  case u9t::c64: return u9Sc<c64, T>::f(R<c64>(data(), i * s));
    TA(0, "i9at internal error, T = " << u9t_<T>::t << ", t = " << t);
  }
}


template<class T>
i9 i9::set(uN i, T x) const
{
  let t = type();
  if (t == u9t::b) return bvec_set(i, u9Sc<T, bool>::f(x));

  u9vectors(t);
  let s = u9sizeof(t);
  A((i + 1) * s <= size(),  // important: final byte is checked
    "i9set OOB, i = " << i << ", t = " << t << ", w = " << s << ", sz = " << size());

  switch (t)
  {
  case u9t::i8:  W(data(), i * s, u9Sc<T, i8> ::f(x)); break;
  case u9t::i16: W(data(), i * s, u9Sc<T, i16>::f(x)); break;
  case u9t::i32: W(data(), i * s, u9Sc<T, i32>::f(x)); break;
  case u9t::i64: W(data(), i * s, u9Sc<T, i64>::f(x)); break;
  case u9t::u8:  W(data(), i * s, u9Sc<T, u8> ::f(x)); break;
  case u9t::u16: W(data(), i * s, u9Sc<T, u16>::f(x)); break;
  case u9t::u32: W(data(), i * s, u9Sc<T, u32>::f(x)); break;
  case u9t::u64: W(data(), i * s, u9Sc<T, u64>::f(x)); break;

  case u9t::f32: W(data(), i * s, u9Sc<T, f32>::f(x)); break;
  case u9t::f64: W(data(), i * s, u9Sc<T, f64>::f(x)); break;
  case u9t::c32: W(data(), i * s, u9Sc<T, c32>::f(x)); break;
  case u9t::c64: W(data(), i * s, u9Sc<T, c64>::f(x)); break;
    TA(0, "i9set internal error, T = " << u9t_<T>::t << ", t = " << t);
  }
  return *this;
}


template<class T>
requires u9t_hastype<T> && u9t_is<T, u9fixed.m>::v
i9::operator T() const { return at<T>(0); }

bool i9::b() const { return at<bool>(0); }


inline i9 i9_false()        { return i9{i9_statics + 0}; }
inline i9 i9_true()         { return i9{i9_statics + 3}; }
inline i9 i9_null()         { return i9{i9_statics + 6}; }
inline i9 i9_no_key()       { return i9{i9_statics + 9}; }
inline i9 i9_tuple_bounds() { return i9{i9_statics + 12}; }
inline i9 i9_invalid()      { return i9{i9_statics + 15}; }
inline i9 i9_none()         { return i9{i9_statics + 18}; }


static_assert(sizeof(i9) == sizeof(uN));


template<class T>
void i9_ssort(T s, T e)
{
  struct hx { u64 h; i9 x; };
  let n = std::distance(s, e);
  V<hx> hs; hs.reserve(n);
  for (T i = s; i < e; i++) hs.push_back(hx{(*i).h(), *i});
  std::sort(hs.begin(), hs.end(), [](hx &a, hx &b) { return a.h < b.h; });
  T i = s;
  for (let h : hs) *i++ = h.x;
}


template<class T>
void i9_msort(T s, T e)
{
  struct hx { u64 h; i9 k; i9 v; };
  let n = std::distance(s, e) / 2;
  V<hx> hs; hs.reserve(n);
  for (T i = s; i < e;)
  { let k = *i++;
    let v = *i++;
    hs.push_back(hx{k.h(), k, v}); }
  std::sort(hs.begin(), hs.end(), [](hx &a, hx &b) { return a.h < b.h; });
  T i = s;
  for (let h : hs) { *i++ = h.k; *i++ = h.v; }
}


#if τdebug_iostream
O &operator<<(O &s, i9v x)
{
  switch (x)
  {
  case i9v::valid:         return s << "OK";
  case i9v::bad_collsize:  return s << "s≠Σ";
  case i9v::bad_element:   return s << "X∈";
  case i9v::osize_2:       return s << "|2";
  case i9v::osize_overrun: return s << "|>os";
  case i9v::osize_sb:      return s << "|>sb";
  case i9v::undersized:    return s << "|<s";
  }
}

O &operator<<(O &s, i9 const &x)
{
  if (τdebug_i9st)
    s << "i9@" << Sc<void*>(x.a) << ":" << x.type() << ":" << x.flagged() << ":" << x.size() << " ";

  switch (x.type())
  {

#define vec(rt, ct, t)                                                  \
    case rt:                                                            \
      if (!x.vn()) return s << x.type() << "[]";                        \
      s << Sc<ct>(x);                                                   \
      for (uN i = 1; i < x.vn(); ++i)                                   \
        s << " " << Sc<ct>(x.at<t>(i));                                 \
      return s;

  vec(u9t::i8,  i64, i8)
  vec(u9t::i16, i64, i16)
  vec(u9t::i32, i64, i32)
  vec(u9t::i64, i64, i64)

  vec(u9t::u8,  i64, u8)
  vec(u9t::u16, i64, u16)
  vec(u9t::u32, i64, u32)
  vec(u9t::u64, u64, u64)

  vec(u9t::f32, f32, f32)
  vec(u9t::f64, f64, f64)
  vec(u9t::c32, c32, c32)
  vec(u9t::c64, c64, c64)

#undef vec

  case u9t::b:
    if (!x.vn()) return s << "b[]";
    s << (x.template at<bool>(0) ? 't' : 'f');
    for (uN i = 1; i < x.vn(); ++i)
      s << ' ' << (x.template at<bool>(i) ? 't' : 'f');
    return s;

  case u9t::symbol: return s << Sc<u9_symbol>(x);
  case u9t::stream:
  { sc chc *exp[10] = {"⁰", "¹", "²", "³", "⁴", "⁵", "⁶", "⁷", "⁸", "⁹"};
    s << Sc<u9st>(x);
    if (x.dim() != 1)
    { V<u8> e; e.reserve(3);
      for (uN y = x.dim(); y; y /= 10) e.push_back(y % 10);
      std::reverse(e.begin(), e.end());
      for (let z : e) s << exp[z]; }

    return x.θ() ? s << Sc<f64>(x.θ()) / Sc<f64>(Nl<u64>::max())
         : x.ι() ? s << x.ι()
         : s; }

  case u9t::bytes:  return s << "b\"" << Stv(Rc<chc*>(x.begin().a), x.size()) << "\"";
  case u9t::utf8:   return s << "\""  << Stv(Rc<chc*>(x.begin().a), x.size()) << "\"";
  case u9t::index:  return s << "i"   << Sc<uN>(x.ibits()) << x.icoll();

  case u9t::tuple:
  { let b = x.begin();
    let e = x.end();
    s << "(";
    for (auto i = 0; b + i < e; i += i9::size_of(b + i))
    { if (i) s << ","; s << i9{b + i}; }
    return s << ")"; }

  case u9t::set:
  { let b = x.begin();
    let e = x.end();
    s << "{";
    for (auto i = 0; b + i < e; i += i9::size_of(b + i))
    { if (i) s << ","; s << i9{b + i}; }
    return s << "}"; }

  case u9t::map:
  { let b = x.begin();
    let e = x.end();
    s << "{";
    for (auto i = 0; b + i < e;)
    { if (i) s << ", ";
      let k = i9{b + i}; s << k << ":"; if (b + (i += k.osize()) >= e) break;
      let v = i9{b + i}; s << v;        i += v.osize(); }
    return s << "}"; }

  case u9t::tensor:
  { let d  = i9{x.begin()};
    let xs = i9{d.end()};
    return s << xs.type() << "[" << d << "]"; }

  case u9t::pi:
  { if (x.is_πref()) return s << "π₀r:" << *x;
    let p = Sc<u9_scoped<u9_π, ζp>>(x);
    return s << "π₀:" << p.t << ":" << p.x; }

  case u9t::none: return s << "none[" << R<u9_none>(x.data(), 0) << "]";

  default:
    return s << "i9[" << x.type() << ":" << x.flagged() << ":" << x.size() << "]";
  }
}
#endif


}


template<>
struct std::hash<τ::i9>
{ inline size_t operator()(τ::i9 const x) const { return x.h(); } };


namespace τ
{


inline M<i9, i9> &i9::into(M<i9, i9> &xs) const
{ u9tm{u9t::map}(type());
  for (let x : keys()) xs[x] = x.next();
  return xs; }


}


#include "end.hh"


#endif
