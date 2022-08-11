#ifndef tau_utf9_hash_h
#define tau_utf9_hash_h


#define XXH_INLINE_ALL 1
#include "../deps/xxhash.h"
#undef XXH_INLINE_ALL

#include "../deps/picosha2.h"


#include "../types.hh"
#include "../util/numerics.hh"
#include "../util/ostream.hh"

#include "ibuf.hh"


#include "../module/begin.hh"

namespace tau::utf9
{

using namespace tau::util::numerics;


static_assert(sizeof(XXH64_hash_t) == sizeof(u64));

inline u64 xxh(void const *x, uN l, u64 s) { return XXH64(x, l, s); }
inline u64 xxc(u64 a, u64 b)               { a = ce(a); return XXH64(&a, sizeof a, b); }


struct sha256
{
  u64 xs[4];

  sha256(V<u8> const &b)
    { xs[0] = ce(*Rc<u64c*>(b.data()));
      xs[1] = ce(*Rc<u64c*>(b.data() + 8));
      xs[2] = ce(*Rc<u64c*>(b.data() + 16));
      xs[3] = ce(*Rc<u64c*>(b.data() + 24)); }

  sha256(ibuf const &b, uN i)
    { xs[0] = b.U64(i);
      xs[1] = b.U64(i + 8);
      xs[2] = b.U64(i + 16);
      xs[3] = b.U64(i + 24); }

  sha256(sha256 const &s)
    { xs[0] = s.xs[0];
      xs[1] = s.xs[1];
      xs[2] = s.xs[2];
      xs[3] = s.xs[3]; }

  bool operator<(sha256 const &s_) const
    { for (uN i = 0; i < 4; ++i)
        if      (xs[i] < s_.xs[i]) return true;
        else if (xs[i] > s_.xs[i]) return false;
      return false; }

  bool operator>(sha256 const &s_) const
    { for (uN i = 0; i < 4; ++i)
        if      (xs[i] > s_.xs[i]) return true;
        else if (xs[i] < s_.xs[i]) return false;
      return false; }

  bool operator==(sha256 const &s_) const
    { return xs[0] == s_.xs[0] && xs[1] == s_.xs[1]
          && xs[2] == s_.xs[2] && xs[3] == s_.xs[3]; }
};


template<class I>
inline sha256 sha(I begin, I end)
{
  V<u8> h(picosha2::k_digest_size);
  picosha2::hash256(begin, end, h.begin(), h.end());
  return sha256(h);
}


template<class T>
inline sha256 sha(T const &x)
{
  return sha(x.begin(), x.end());
}


#if tau_debug_iostream
O &operator<<(O &s, sha256 const &h)
{
  using tau::util::hexout;
  return hexout(hexout(hexout(hexout(s, h.xs[0]) << ":", h.xs[1]) << ":", h.xs[2]) << ":", h.xs[3]);
}
#endif


}


template<> struct std::hash<tau::utf9::sha256>
{ uint64_t operator()(tau::utf9::sha256 const &v) const { return v.xs[0] ^ v.xs[1] ^ v.xs[2] ^ v.xs[3]; } };


#include "../module/end.hh"


#endif
