#ifndef τutf9_i9_h
#define τutf9_i9_h


#include <cstring>
#include <memory>


#include "debug.hh"
#include "types.hh"
#include "zeta.hh"

#include "utf9-types.hh"

#include "begin.hh"

namespace τ
{


static u8 i9_statics[] =
{
  u9t::b      | u9s::v8, 1, 0,                                    // offset 0 = false
  u9t::b      | u9s::v8, 1, 1,                                    // offset 3 = true
  u9t::symbol | u9s::v8, 1, 0,                                    // offset 6 = null symbol
  u9t::none   | u9s::v8, 1, Sc<u8>(u9_none::key_lookup_missing),  // offset 9 = no key
  u9t::none   | u9s::v8, 1, Sc<u8>(u9_none::tuple_bounds_error),  // offset 12 = tuple OOB
};


struct i9;
inline i9 i9_false();
inline i9 i9_true();
inline i9 i9_null();
inline i9 i9_none();
inline i9 i9_no_key();
inline i9 i9_tuple_bounds();


struct i9
{
  static inline uN   size_of(ζp a) { return u9rs(a, 0); }
  static inline i9   cast   (ζp a) { return i9{a}; }
  static inline void free   (ζp a) { i9 i{a}; if (i.type() == u9t::heapref) i.free(); }


  struct it
  {
    ζp a;
    operator     ζp()            const { return a; }
    i9   operator* ()            const { return i9{a}; }
    it   operator++()                  { a += i9{a}.osize(); return *this; }
    bool operator==(it const &x) const { return a == x.a; }
  };


  struct kit  // key iterator
  {
    it i;
    i9   operator* ()             const { return *i; }
    kit  operator++()                   { ++i; ++i; return *this; }
    bool operator==(kit const &x) const { return i == x.i; }
  };

  struct ks
  {
    ζp a;
    kit begin() const { return kit{it{a + u9sb(u9ts_s(R<u8>(a, 0)))}}; }
    kit end()   const { return kit{it{a + u9rs(a, 0)}}; }
  };


  ζp a;
  i9(ζp a_) : a(a_) {}

  // UTF9 values are truthy if they are normal, that is they (1) exist,
  // and (2) are not stream metadata
  operator bool() const { return exists() && type() != u9t::stream; }
  bool   exists() const { return a && a != ζωp; }
  operator   ζp() const { return a; }


  uf8  code()    const { return R<u8>(a, 0); }
  bool flagged() const { return u9ts_f(code()); }
  u9s  stype()   const { return u9ts_s(code()); }
  u9t  type()    const { return a ? u9ts_t(code()) : u9t::none; }
  it   begin()   const { return it{a + u9sb(stype())}; }
  it   end()     const { return it{a + u9rs(a, 0)}; }
  i9   next()    const { return i9{a + osize()}; }
  ks   keys()    const { return ks{a}; }
  uN   osize()   const { return u9rs(a, 0); }


  // NOTE: inner, "logical" size, not outer size; that way these methods
  // have STL-style meanings
  ζp   data()    const { return begin(); }
  uN   size()    const { return end() - begin(); }

  // NOTE: returns 0 if this type cannot be vectorized
  uN   vn()      const { return size() >> u9logsizeof(type()); }


  template<class T>
  requires u9t_hastype<T> && u9t_is<T, u9fixed.m>::v && (!u9t_is<T, u9signed.m>::v)
    explicit operator T() const { u9tm{u9t_<T>::t}(type()); return R<T>(begin(), 0); }

  explicit operator i64() const
    { u9ints(type());
      switch (type())
      {
      case u9t::i8:  return R<i8> (begin(), 0);
      case u9t::i16: return R<i16>(begin(), 0);
      case u9t::i32: return R<i32>(begin(), 0);
      case u9t::i64: return R<i64>(begin(), 0);
      case u9t::u8:  return R<u8> (begin(), 0);
      case u9t::u16: return R<u16>(begin(), 0);
      case u9t::u32: return R<u32>(begin(), 0);
      case u9t::u64: return R<u64>(begin(), 0);
        TA(0, type());
      } }

  explicit operator i32() const
    { u9ints(type());
      switch (type())
      {
      case u9t::i8:  return          R<i8> (begin(), 0);
      case u9t::i16: return          R<i16>(begin(), 0);
      case u9t::i32: return          R<i32>(begin(), 0);
      case u9t::i64: return coi<i32>(R<i64>(begin(), 0));
      case u9t::u8:  return          R<u8> (begin(), 0);
      case u9t::u16: return          R<u16>(begin(), 0);
      case u9t::u32: return coi<i32>(R<u32>(begin(), 0));
      case u9t::u64: return coi<i32>(R<u64>(begin(), 0));
        TA(0, type());
      } }

  template<class T>
  T const *operator*() const
    { u9tm{u9t::nstruct}(type());
      A(sizeof(T) <= size(), "i9 native T* overflows bounds; |T|=" << sizeof(T) << ", size()=" << size());
      return Rc<T const*>(data()); }

  template<class T>
  T *operator*() const
    { u9tm{u9t::nstruct}(type());
      A(sizeof(T) <= size(), "i9 native T* overflows bounds; |T|=" << sizeof(T) << ", size()=" << size());
      return Rc<T*>(data()); }

  operator St       () const { return St{Rc<ch*>(data()), size()}; }
  operator Bv       () const { return Bv{data(), size()}; }
  operator u9_symbol() const { u9tm{u9t::symbol}(type()); return u9_symbol{B(data(), size())}; }

  operator u9st() const
    { u9tm{u9t::stream}(type());
      return Sc<u9st>(R<u8>(begin(), 0)); }

  u64  θ() const { return type() == u9t::stream && Sc<u9st>(*this) == u9st::θ ? R<u64>(begin(), 1) : 0; }
  u64  ι() const { return type() == u9t::stream && Sc<u9st>(*this) == u9st::ι ? R<u64>(begin(), 1) : 0; }
  bool τ() const { return type() == u9t::stream && Sc<u9st>(*this) == u9st::τ; }

  uN   π() const { u9tm{u9t::pi}(type()); return *Rc<uNc*>(data()); }


  i9 operator*() const
    { u9tm{u9t::heapref, u9t::heappin}(type());
      return i9{Rc<ζp>(R<u9_heapref>(begin(), 0).r)}; };

  i9 &free()
    { if (type() == u9t::heapref || type() == u9t::heappin) std::free((**this).a);
      return *this; }

  i9 &pin()
    { if (type() == u9t::heapref) W<u8>(a, 0, u9t::heappin | stype());
      return *this; }


  template<class T>
  T at(uN i) const
    { u9tm{u9t_<T>::t}(type());
      A((i + 1) * sizeof(T) <= size(),
        "i9at OOB, i = " << i << ", w = " << sizeof(T) << ", sz = " << size());
      return R<T>(begin(), i * sizeof(T)); }


  // NOTE: can't do vectors here because we don't have enough space
  // to return type information alongside the value; use .at()
  i9 operator[](uN i) const
    { u9tm{u9t::index, u9t::tuple}(type());
      switch (type())
      {
      case u9t::index: TODO("i9[uN] u9t::index");
      case u9t::tuple: { ζp b = data(); while (i--) b += size_of(b); return b; }
        TA(0, "i9[uN] requires index or tuple, not " << type())
      } }

  i9 operator[](i9 i) const
    { u9tm{u9t::tuple, u9t::set, u9t::map, u9t::index}(type());
      switch (type())
      {
      case u9t::tuple:
      { iN  n = Sc<iN>(i);
        let u = size();
        uN  j = 0;
        for (; n && j < u; --n, j += i9::size_of(data() + j));
        return n ? i9_none() : i9{data() + j}; }

      case u9t::set:
      { for (let x : *this) if (i == x) return i9_true();
        return i9_false(); }

      case u9t::map:
      { for (let x : keys()) if (i == x) return x.next();
        return i9_no_key(); }

      case u9t::index: TODO("i9[i9] index");

      TA(*this, "i9[i9] undefined for " << type());
      } }


  bool operator==(uN   x) const { return u9unsigned[type()] && Sc<uN> (*this) == x; }
  bool operator==(i64  x) const { return u9signed[type()]   && Sc<i64>(*this) == x; }
  bool operator==(i32  x) const { return u9signed[type()]   && Sc<i32>(*this) == x; }
  bool operator==(Stc &x) const { return u9strings[type()]  && size() == x.size() && !std::memcmp(data(), x.data(), x.size()); }
  bool operator==(Bc  &x) const { return u9strings[type()]  && size() == x.size() && !std::memcmp(data(), x.data(), x.size()); }
  bool operator==(i9   x) const { return x.a == a || type() == x.type() && size() == x.size() && !std::memcmp(data(), x.data(), x.size()); }
  bool operator==(ζp   b) const { return a == b; }


  bool verify(uN s) const  // verify this value with the given external size
    { // TODO: verify that this value can be decoded safely
      TODO("i9 verify()"); }
};


inline i9 i9_false()        { return i9{i9_statics + 0}; }
inline i9 i9_true()         { return i9{i9_statics + 3}; }
inline i9 i9_null()         { return i9{i9_statics + 6}; }
inline i9 i9_no_key()       { return i9{i9_statics + 9}; }
inline i9 i9_tuple_bounds() { return i9{i9_statics + 12}; }
inline i9 i9_none()         { return i9{ζωp}; }


static_assert(sizeof(i9) == sizeof(uN));


#if τdebug_iostream
O &operator<<(O &s, i9 const &x)
{
  if (τdebug_i9st)
    s << "i9@" << Sc<void*>(x.a) << ":" << x.type() << ":" << x.flagged() << ":" << x.size() << " ";

  switch (x.type())
  {

#define vec(rt, ct, t)                                                  \
    case rt:                                                            \
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

  case u9t::pidfd:  return s << "pidfd(" << Sc<u9_pidfd>(x).pid << "," << Sc<u9_pidfd>(x).fd << ")";
  case u9t::b:      return s << (R<u8>(x.begin(), 0) ? "t" : "f");
  case u9t::symbol: return s << Sc<u9_symbol>(x);
  case u9t::stream:
    s << Sc<u9st>(x);
    return x.θ() ? s << Sc<f64>(x.θ()) / Sc<f64>(Nl<u64>::max())
         : x.ι() ? s << x.ι()
         : s;

  case u9t::bytes:  return s << "b\"" << Stv(Rc<chc*>(x.begin().a), x.size()) << "\"";
  case u9t::utf8:   return s << "\""  << Stv(Rc<chc*>(x.begin().a), x.size()) << "\"";
  case u9t::index:  return s << "i"   << i9{x.begin()};

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
      let k = i9{b + i}; s << k << ":"; i += k.osize();
      let v = i9{b + i}; s << v;        i += v.osize(); }
    return s << "}"; }

  case u9t::tensor:
  { let d  = i9{x.begin()};
    let xs = i9{d.end()};
    return s << xs.type() << "[" << d << "]"; }

  case u9t::heapref: return s << "heap@" << Rc<void*>((*x).a);
  case u9t::heappin: return s << "hpin@" << Rc<void*>((*x).a);

  case u9t::pi: return s << "π₀[" << *Rc<uN*>(x.data()) << "]";

  default:
    return s << "i9[" << x.type() << ":" << x.flagged() << ":" << x.size() << "]";
  }
}
#endif


}

#include "end.hh"


#endif
