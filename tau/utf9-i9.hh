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
  static uN size_of(ζp a)
    { switch (u9ts_s(R<u8>(a, 0)))
      {
      case u9s::f1:  return 1 + 1;
      case u9s::f2:  return 1 + 2;
      case u9s::f4:  return 1 + 4;
      case u9s::f8:  return 1 + 8;
      case u9s::v8:  return 2 + R<u8> (a, 1);
      case u9s::v16: return 3 + R<u16>(a, 1);
      case u9s::v32: return 5 + R<u32>(a, 1);
      case u9s::v64: return 9 + R<u64>(a, 1);
        TA(0)
      } }


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
  uN  size()  const { return size_of(a); }
  uN  osize() const { return end() - a; }


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

  operator u8()  const { u9tm{u9t::u8} (type()); return R<u8> (begin(), 0); }
  operator u16() const { u9tm{u9t::u16}(type()); return R<u16>(begin(), 0); }
  operator u32() const { u9tm{u9t::u32}(type()); return R<u32>(begin(), 0); }
  operator u64() const { u9tm{u9t::u64}(type()); return R<u64>(begin(), 0); }

  operator f32() const { u9tm{u9t::f32}(type()); return R<f32>(begin(), 0); }
  operator f64() const { u9tm{u9t::f64}(type()); return R<f64>(begin(), 0); }
};


static_assert(sizeof(i9) == sizeof(uN));


#if tau_debug_iostream
O &operator<<(O &s, i9 const &i)
{
  return s << "u9[" << i.type() << ":" << i.size() << "]";
}
#endif


}

#include "end.hh"


#endif
