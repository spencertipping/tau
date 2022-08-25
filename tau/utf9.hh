#ifndef tau_utf9_h
#define tau_utf9_h


#include "types.hh"
#include "zeta.hh"

#include "begin.hh"

namespace tau
{


struct i9
{
  u8 *zr;
  i9(u8 *zr_) : zr(zr_) {}

  //bool  exists() const { return zr != Nl<uN>::max(); }
  //u8   *start()  const { return rζ<i9>(zr) + ζxi(zr); }
  bool exists() const { return Rc<uN>(zr) != -1; }
  u8  *start()  const { return zr; }

  static void free   (u8 *) {}
  static uN   size_of(u8c *a)
    { switch (R<u8>(a, 0) & 7)
      {
      case 0: return 1 + 1;
      case 1: return 1 + 2;
      case 2: return 1 + 4;
      case 3: return 1 + 8;
      case 4: return 2 + R<u8>(a, 1);
      case 5: return 3 + R<u16>(a, 1);
      case 6: return 5 + R<u32>(a, 1);
      case 7: return 9 + R<u64>(a, 1);
      default: assert(0);
      } }
};


template<class T>
struct o9n
{
  T x;

  uN   size (uN, uN)    const { return 1 + sizeof(T); }
  void write(u8 *m, uN) const
    { switch (sizeof(T))
      {
      case 1: W<u8>(m, 0, 0x20); break;
      case 2: W<u8>(m, 0, 0x21); break;
      case 4: W<u8>(m, 0, 0x22); break;
      case 8: W<u8>(m, 0, 0x23); break;
      }
      W<T>(m, 1, x); }
};


}

#include "end.hh"


#endif
