#ifndef tau_utf9_i9_h
#define tau_utf9_i9_h


#include <memory>


#include "debug.hh"
#include "types.hh"
#include "zeta.hh"

#include "utf9-types.hh"

#include "begin.hh"

namespace tau
{


struct i9
{
  static uN size_of(ζp a) { return u9rs(a, 0); }
  static i9 cast   (ζp a) { return i9{a}; }

  struct it
  {
    ζp a;
    operator   ζp() const { return a; }
    i9 operator* () const { return i9{a}; }
    it operator++()       { a += i9{a}.size(); return *this; }
  };


  ζp a;
  i9(ζp a_) : a(a_) {}

  bool exists() const { return a != ζωp; }
  operator ζp() const { return a; }


  uf8 code()  const { return R<u8>(a, 0); }

  u9s stype() const { return u9ts_s(code()); }
  u9t type()  const { return a ? u9ts_t(code()) : u9t::none; }
  it  begin() const { return it{a + u9sb(stype())}; }
  it  end()   const { return it{a + u9rs(a, 0)}; }

  // NOTE: inner, "logical" size, not outer size
  ζp  data()  const { return begin(); }
  uN  size()  const { return end() - begin(); }


  // NOTE: returns 0 if this type cannot be vectorized
  uN  vn()    const { return size() >> u9logsizeof(type()); }


  template<class T>
  requires u9t_hastype<T> && u9t_is<T, u9fixed.m>::v && (!u9t_is<T, u9signed.m>::v)
    operator T() const { u9tm{u9t_<T>::t}(type()); return R<T>(begin(), 0); }

  operator i64() const
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
        TA(0, type())
      } }

  operator u9st() const
    { u9tm{u9t::stream}(type());
      return Sc<u9st>(R<u8>(begin(), 0)); }

  u64 θ() const
    { u9tm{u9t::stream}(type());
      return Sc<u9st>(*this) == u9st::θ ? R<u64>(begin(), 1) : 0; }


  i9 operator*() const
    { u9tm{u9t::heapref}(type());
      return i9{Rc<ζp>(R<u9_heapref>(begin(), 0).r)}; };

  void free() { std::free((*this).a); a = nullptr; }


  i9 operator[](uN i) const
    { switch (type())
      {
        // TODO: tuple indexes
      case u9t::tuple: { ζp b = begin(); while (i--) b += size_of(b); return b; }
        TA(0, "i9[uN] requires index or tuple, not " << type())
      } }

  i9 operator[](i9 i) const
    { // TODO: indexes
      // TODO: sets
      // TODO: maps
      A(0, "TODO i9[]");
      return *this;
    }
};


static_assert(sizeof(i9) == sizeof(uN));


#if tau_debug_iostream
O &operator<<(O &s, i9 const &x)
{
  if (tau_debug_i9st)
    s << "i9@" << Sc<void*>(x.a) << ":" << x.type() << ":" << x.size() << " ";

  switch (x.type())
  {

#define vec(rt, ct, t)                                                  \
    case rt:                                                            \
      s << Sc<ct>(x);                                                   \
      for (uN i = 1; i < x.vn(); ++i)                                   \
        s << " " << Sc<ct>(R<t>(x.begin(), i * u9sizeof(rt)));          \
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

  case u9t::b:      return s << (R<u8>(x.begin(), 0) ? "t" : "f");
  case u9t::symbol: return s << "TODO: i9 symbol";
  case u9t::stream:
    s << Sc<u9st>(x);
    return x.θ() ? s << Sc<f64>(x.θ()) / Sc<f64>(Nl<u64>::max()) : s;

  case u9t::bytes:  return s << "b\"" << Stv(Rc<chc*>(x.begin().a), x.size()) << "\"";
  case u9t::utf8:   return s << "u\"" << Stv(Rc<chc*>(x.begin().a), x.size()) << "\"";
  case u9t::index:  return s << "i" << i9{x.begin()};

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
      let k = i9{b + i}; s << k << ":"; i += k.size();
      let v = i9{b + i}; s << v; i += v.size(); }
    return s << "}"; }

  case u9t::tensor:
  { let d  = i9{x.begin()};
    ζp  xs = i9{d}.end();
    let e  = x.end();

    return s << "TODO: i9 tensor";

    // FIXME: these are vectorized, not sequential tuples
    uN ds[d.vn()]; uN di = 0; for (let x : d) ds[di++] = Sc<i64>(x);
    uN is[d.vn()];            for (uN i = 0; i < d.vn(); ++i) is[i] = 0;
  }

  case u9t::heapref: return s << "heap@" << Rc<void*>((*x).a);

  default:
    return s << "i9[" << x.type() << ":" << x.size() << "]";
  }
}
#endif


}

#include "end.hh"


#endif
