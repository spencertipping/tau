#include "eta-i.hh"
#include "begin.hh"

namespace τ
{


PO ηscmp(ηi a, ηi b)
{
  for (;; ++a, ++b)
  {
    let c = a <=> b;
    if (c != PO::equivalent)            return c;
    if (!a.has_next() && !b.has_next()) return PO::equivalent;
    if (!a.has_next())                  return PO::less;
    if (!b.has_next())                  return PO::greater;
  }
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
  let tc = t() <=> x.t();
  if (tc != SO::equal) return tc;
  switch (t())
  {
  case ηtype::sig:      return sig() <=> x.sig();
  case ηtype::n_int:    return i()   <=> x.i();
  case ηtype::n_float:  return f()   <=> x.f();
  case ηtype::string:   return s()   <=> x.s();
  case ηtype::atom:     return a()   <=> x.a();
  case ηtype::name:     return n()   <=> x.n();

  case ηtype::int8s:    return i8s()  <=> x.i8s();
  case ηtype::int16s:   return i16s() <=> x.i16s();
  case ηtype::int32s:   return i32s() <=> x.i32s();
  case ηtype::int64s:   return i64s() <=> x.i64s();
  case ηtype::float32s: return f32s() <=> x.f32s();
  case ηtype::float64s: return f64s() <=> x.f64s();

  case ηtype::η: return ηscmp(*this, x);

  default: return PO::unordered;
  }
}


Sn<u8c> ηi::at(uN i) const
{
  ηi r = *this;
  while (i && has_next()) r = r.next(), --i;
  return {r.a_, i ? 0 : r.l_};
}

Sn<u8c> ηi::at(ηname const &n) const
{
  ηi r = *this;
  for (; r.has_next(); r = r.next())
    if (r.is_n(n.x)) return r.after();
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
  case ηtype::int8s:    return s << "i8s[" << i.size() << "]";
  case ηtype::int16s:   return s << "i16s[" << i.size() / 2 << "]";
  case ηtype::int32s:   return s << "i32s[" << i.size() / 4 << "]";
  case ηtype::int64s:   return s << "i64s[" << i.size() / 8 << "]";
  case ηtype::float32s: return s << "f32s[" << i.size() / 4 << "]";
  case ηtype::float64s: return s << "f64s[" << i.size() / 8 << "]";
  case ηtype::η:        return s << "(" << i.η() << ")";
  default:
    return s << "ηi[t=" << Sc<int>(i.t()) << ", s=" << i.size() << "]";
  }
}

O &operator<<(O &s, ηi i)
{
  yone(s, i);
  while (i.has_next()) yone(s << " ", i = i.next());
  return s;
}


}

#include "end.hh"
