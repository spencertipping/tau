#ifndef tau_utf9_o9_h
#define tau_utf9_o9_h


#include "debug.hh"
#include "types.hh"
#include "zeta.hh"

#include "utf9-types.hh"

#include "begin.hh"

namespace tau
{


template<class T> struct o9t;

template<> struct o9t<u8>  { static constexpr u9t const type = u9t::u8;  };
template<> struct o9t<u16> { static constexpr u9t const type = u9t::u16; };
template<> struct o9t<u32> { static constexpr u9t const type = u9t::u32; };
template<> struct o9t<u64> { static constexpr u9t const type = u9t::u64; };


template<class T>
struct o9
{
  T x;

  uN   size (uN, uN)   const { return 1 + sizeof(T); }
  void write(ζp m, uN) const { W<T>(m, u9ws(m, 0, o9t<T>::type, sizeof(T)), x); }
};


}

#include "end.hh"


#endif
