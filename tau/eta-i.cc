#define XXH_INLINE_ALL
#include <xxhash.h>
#include <picosha2.h>

#include "eta-i.hh"
#include "begin.hh"

namespace τ
{


u64 ηi::hash() const
{
  return XXH64(ldata(), lsize(), 0);
}


h256 ηi::sha256() const
{
  h256 r;
  picosha2::hash256(ldata(), ldata() + lsize(), r.begin(), r.end());
  return r;
}


// Result bit ordering: 0xnedcba9876543210 -- where 0 is the first type, etc.
// Unspecified types are 0xf. The most-significant four bits encode the number
// of values in the set.
ηtl ηi::ts() const
{
  u64 r = 0x0fffffffffffffffull;
  ηi  i = *this;
  uN  n = 0;
  while (i.has_next() && n < 15)
  {
    r = r & ~(0xf << n * 4) | u64(i.t()) << n * 4;
    ++i;
    ++n;
  }
  return {r | u64(n) << 60};
}


PO ηi::operator<=>(ηic &x) const
{
  if (!*this && !x) return PO::equivalent;
  if (!*this)       return PO::less;
  if (!x)           return PO::greater;

  let tc = t() <=> x.t();
  if (tc != SO::equal) return tc;

  PO r = PO::equivalent;
  switch (t())
  {
  case ηtype::sig:      r = sig()  <=> x.sig();  break;
  case ηtype::n_int:    r = i()    <=> x.i();    break;
  case ηtype::n_float:  r = f()    <=> x.f();    break;
  case ηtype::string:   r = s()    <=> x.s();    break;
  case ηtype::atom:     r = a()    <=> x.a();    break;
  case ηtype::name:     r = n().x  <=> x.n().x;  break;
  case ηtype::binary:   r = bin()  <=> x.bin();  break;
  case ηtype::int8s:    r = i8s()  <=> x.i8s();  break;
  case ηtype::int16s:   r = i16s() <=> x.i16s(); break;
  case ηtype::int32s:   r = i32s() <=> x.i32s(); break;
  case ηtype::int64s:   r = i64s() <=> x.i64s(); break;
  case ηtype::float32s: r = f32s() <=> x.f32s(); break;
  case ηtype::float64s: r = f64s() <=> x.f64s(); break;
  case ηtype::η:        r = η()    <=> x.η();    break;

  // NOTE: ext data is always unordered
  default:              r = PO::unordered;       break;
  }

  return r == PO::equivalent ? next() <=> x.next() : r;
}


ηi ηi::operator[](uN i) const
{
  ηi r = *this;
  while (i && r.has_next()) ++r, --i;
  return {r.a_, i ? 0 : r.l_};
}

ηi ηi::operator[](ηname const &n) const
{
  ηi r = *this;
  for (; r.has_next(); r = r.next())
    if (r.is_n(n.x)) return r.next();
  return {r.a_, 0};
}


T<ηicb_r, uN, u8> ηicb(u8c *a, uN l)
{
  if (!l) return {ηicb_r::no_ctrl, 0, 0};
  let sn = *a & 0x0f;
  uN  s;  // decoded size
  u8  c;  // number of control+size bytes

  if      (sn <= 0x0c) c = 1, s = sn;
  else if (sn == 0x0d)
  {
    if (l < 2) return {ηicb_r::short_len1, 0, 0};
    c = 2, s = 13 + a[1];
  }
  else if (sn == 0x0e)
  {
    if (l < 3) return {ηicb_r::short_len2, 0, 0};
    c = 3, s = 269 + (u16(a[1]) << 8 | a[2]);
  }
  else
  {
    if (l < 5) return {ηicb_r::short_len4, 0, 0};
    c = 5;
    s = 65805 + (  u32(a[1]) << 24
                 | u32(a[2]) << 16
                 | u32(a[3]) << 8
                 |     a[4]);
  }

  // NOTE: this looks weird, but we're making sure that we aren't
  // overflowing unsigned int math so the l check is valid.
  if (s + c <= s) return {ηicb_r::int_overflow, 0, 0};
  if (s + c > l)  return {ηicb_r::size_overflow, 0, 0};
  return {ηicb_r::ok, s, c};
}


O &operator<<(O &s, ηicb_r x)
{
  switch (x)
  {
  case ηicb_r::ok:            return s << "ok";
  case ηicb_r::no_ctrl:       return s << "no_ctrl";
  case ηicb_r::short_len1:    return s << "short_len1";
  case ηicb_r::short_len2:    return s << "short_len2";
  case ηicb_r::short_len4:    return s << "short_len4";
  case ηicb_r::int_overflow:  return s << "int_overflow";
  case ηicb_r::size_overflow: return s << "size_overflow";
  default:
    return s << "ηicb_r(" << Sc<int>(x) << ")";
  }
}


Tt static O &pspan(O &s, Sn<T> const &xs)
{
  s << "[";
  for (auto i = xs.begin(); i != xs.end(); ++i)
  {
    if (i != xs.begin()) s << " ";
    s << widen(*i);
  }
  return s << "]";
}


static O &hexout(O &s, Stvc &x)
{
  for (let c : x)
    s << "0123456789abcdef"[u8(c) >> 4 & 0x0f]
      << "0123456789abcdef"[u8(c)      & 0x0f];
  return s;
}


static O &yone(O &s, ηic &i)
{
  switch (i.t())
  {
  case ηtype::sig:      return s << i.sig();
  case ηtype::n_int:    return s << i.i();
  case ηtype::n_float:  return s << i.f();
  case ηtype::string:   return s << "\"" << i.s() << "\"";
  case ηtype::atom:     return s << i.a();
  case ηtype::name:     return s << i.n() << ":";
  case ηtype::binary:   return hexout(i.size() > 64 ? s << "bin[" << i.size() << "] = " : s, i.bin());
  case ηtype::int8s:    return pspan(s << "i8s[" << i.size() << "] = ", i.i8s());
  case ηtype::int16s:   return pspan(s << "i16s[" << i.size() / 2 << "] = ", i.i16s());
  case ηtype::int32s:   return pspan(s << "i32s[" << i.size() / 4 << "] = ", i.i32s());
  case ηtype::int64s:   return pspan(s << "i64s[" << i.size() / 8 << "] = ", i.i64s());
  case ηtype::float32s: return pspan(s << "f32s[" << i.size() / 4 << "] = ", i.f32s());
  case ηtype::float64s: return pspan(s << "f64s[" << i.size() / 8 << "] = ", i.f64s());
  case ηtype::η:        return s << "(" << i.η() << ")";
  case ηtype::ext:      return s << "ext[" << i.size() << "]";
  default:
    return s << "ηi[t=" << Sc<int>(i.t()) << ", s=" << i.size() << "]";
  }
}

O &operator<<(O &s, ηi i)
{
  if (!i) return s;
  yone(s, i);
  while (i.has_next()) yone(s << " ", i = i.next());
  return s;
}


}

#include "end.hh"
