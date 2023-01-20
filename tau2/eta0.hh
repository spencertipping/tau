#ifndef τη0_h
#define τη0_h


#include "../dep/picosha3.h"


#include "types.hh"

#include "begin.hh"

namespace τ
{


typedef u8 η0t;


// η₀ frame type (not exported, just for internal logic):
// short, medium, long, disk
enum class η0ft : u8 { s, m, l, d };


// Bounds-check for a η₀ structure at the given address with the specified
// size bound -- returns true if the structure fits within that bound
bool η0bc(u8c*, uN);


// η₀ structure input reader: decodes the frame and provides .data() and
// .size(), as well as metadata
struct η0i
{
  u8c  *const a;   // pointer to control byte
  η0ft  const ft;  // frame type (cached)
  u8c         hs;  // header size -- *a + hs == data
  η0t   const t;   // type ID (cached)

  η0i(u8c *a_)
    : a(a_), ft(decode_ft()), hs(calculate_hs()), t(decode_type()) {}

  uN   hsize() const { return hs; }
  uN   osize() const { return hs + size(); }
  u8c *data()  const { return a + hs; }
  η0t  type()  const { return t; }
  uN   size()  const
    { switch (ft)
      {
      case η0ft::s: return 1 << (*a & 3);
      case η0ft::m: return a[1] & 127;
      case η0ft::l:
      case η0ft::d:
      { uN s = 0;
        for (uN i = 0; i < (*a & 7) + 1; ++i) s = s << 8 | a[2 + i];
        return s; }
      } }

  bool is_f() const
    { switch (ft)
      {
      case η0ft::s: return !t;
      case η0ft::m: return a[1] & 128;
      case η0ft::l: return *a & 16;
      case η0ft::d: return false;
      } }

  bool is_c() const
    { switch (ft)
      {
      case η0ft::s: case η0ft::m: return false;
      case η0ft::l: case η0ft::d: return a[0] & 8;
      } }

  bool is_h() const
    { switch (ft)
      {
      case η0ft::s:
      case η0ft::m:
      case η0ft::l: return false;
      case η0ft::d: return *a & 16;
      } }

  bool is_v() const
    { if (!is_h()) return true;
      auto sha3_256 = picosha3::get_sha3_generator<256>();
      Ar<u8, 32> hv{};
      sha3_256(begin(), end(), hv.begin(), hv.end());
      return !memcmp(data() - 32, hv.data(), 32); }

  u8c *begin() const { return data(); }
  u8c *end()   const { return data() + size(); }


protected:
  η0ft decode_ft() const
    { if (!(*a & 128))  return η0ft::s;
      if (*a >> 6 == 2) return η0ft::m;
      if (*a >> 5 == 6) return η0ft::l;
      return η0ft::d; }

  u8 calculate_hs() const
    { switch (ft)
      {
      case η0ft::s: return 1;
      case η0ft::m: return 2;
      case η0ft::l: return 2 + (*a & 7);
      case η0ft::d: return 2 + (*a & 7) + (is_h() ? 32 : 0);
      } }

  η0t decode_type() const
    { switch (ft)
      {
      case η0ft::s: return *a >> 2;
      case η0ft::m: return *a & 63;
      case η0ft::l:
      case η0ft::d: return a[1];
      } }
};


static_assert(sizeof(η0i) <= 2 * sizeof(uN));


bool η0bc(u8c *a, uN s)
{
  if (!s)           return false;
  if (!(*a & 127))  return s >= 1 << (*a & 3);
  if (*a >> 6 == 2) return s >= 2 && s >= (a[1] & 127);
  return s >= 2 && s >= 2 + (*a & 7) && s >= η0i(a).osize();
}


#if τdebug_iostream
O &operator<<(O &s, η0ft t)
{
  switch (t)
  {
  case η0ft::s: return s << "s";
  case η0ft::m: return s << "m";
  case η0ft::l: return s << "l";
  case η0ft::d: return s << "d";
  }
}

O &operator<<(O &s, η0i const &i)
{
  return s << "η₀[" << i.ft
           << (i.is_f() ? "F" : "f")
           << (i.is_c() ? "C" : "c")
           << (i.is_h() ? "H" : "h")
           << " t=" << Sc<uN>(i.type())
           << " s=" << i.size() << "]";
}
#endif


}

#include "end.hh"

#endif
