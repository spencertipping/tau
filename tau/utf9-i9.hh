#ifndef tau_utf9_i9_h
#define tau_utf9_i9_h


#include <complex>


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


  ζpc a;
  i9(ζp a_) : a(a_) {}

  bool exists() const { return a != ζωp; }
  operator ζp() const { return a; }


  uf8 code()  const { return R<u8>(a, 0); }

  u9s stype() const { return u9ts_s(code()); }
  u9t type()  const { return u9ts_t(code()); }
  ζp  begin() const { return a + u9sb(stype()); }
  ζp  end()   const { return begin() + size(); }

  ζp  data()  const { return begin(); }
  uN  size()  const { return u9rs(a, 0); }
  uN  osize() const { return end() - a; }


  // NOTE: returns 0 if this type cannot be vectorized
  uN  vn()    const { return size() >> u9logsizeof(type()); }

  operator i64() const
    { u9signed(type());
      switch (type())
      {
      case u9t::i8:  return R<i8> (begin(), 0);
      case u9t::i16: return R<i16>(begin(), 0);
      case u9t::i32: return R<i32>(begin(), 0);
      case u9t::i64: return R<i64>(begin(), 0);
        TA(0)
      } }

  template<class T>
  requires u9t_hastype<T> && (!u9t_is<T, u9signed.m>::v)
  operator T() const { u9tm{u9t_<T>::t}(type()); return R<T>(begin(), 0); }


  i9 operator[](uN i) const
    { switch (type())
      {
      case u9t::tuple: { ζp b = begin(); while (i--) b += size_of(b); return b; }
      default: A(0, "i9[uN] requires index or tuple, not " << type()); return 0;
      } }
};


static_assert(sizeof(i9) == sizeof(uN));


#if tau_debug_iostream
O &operator<<(O &s, i9 const &x)
{
  switch (x.type())
  {
  case u9t::i8:
  case u9t::i16:
  case u9t::i32:
  case u9t::i64: return s << Sc<i64>(x);

  case u9t::u8:  return s << Sc<uN>(Sc<u8>(x));
  case u9t::u16: return s << Sc<u16>(x);
  case u9t::u32: return s << Sc<u32>(x);
  case u9t::u64: return s << Sc<u64>(x);

  case u9t::f32: return s << Sc<f32>(x);
  case u9t::f64: return s << Sc<f64>(x);
  case u9t::c32: return s << Sc<c32>(x);
  case u9t::c64: return s << Sc<c64>(x);

  case u9t::tuple:
  { let b = x.begin();
    let e = x.end();
    s << "tuple[begin=" << b - x.a << " end=" << e - x.a << "]";
    s << "(";
    for (auto i = 0; b + i < e; i += i9::size_of(b + i))
    { if (i) s << ","; s << i9{b + i}; }
    return s << ")"; }

  default:
    return s << "i9[" << x.type() << ":" << x.size() << "]";
  }
}
#endif


}

#include "end.hh"


#endif
