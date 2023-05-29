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
ηts ηi::ts() const
{
  u64 r = 0x0fffffffffffffffull;
  ηi  i = *this;
  uN  n = 0;
  while (i.has_next() && n < 15)
  {
    r = r & ~(0xf << n * 4) | Sc<u64>(i.t()) << n * 4;
    ++i;
    ++n;
  }
  return {r | n << 60};
}


PO ηi::operator<=>(ηi const &x) const
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

Sn<u8c> ηi::at(chc *s) const
{
  return at(St{s});
}

Sn<u8c> ηi::at(Stc &k) const
{
  ηi r = *this;
  for (; r.has_next(); r = r.next())
    if (r.is_n(k)) return r.after();
  return {r.a_, 0};
}

Sn<u8c> ηi::at(Stvc &k) const
{
  ηi r = *this;
  for (; r.has_next(); r = r.next())
    if (r.is_n(k)) return r.after();
  return {r.a_, 0};
}


void ηi::decode_cb()
{
  A(l_ > 0, "η control byte OOB: l = 0");
  let sn = *a_ & 0x0f;

  if      (sn <= 0x0c) c_ = 1, s_ = sn;
  else if (sn == 0x0d)
  {
    A(l_ >= 3, "η control size OOB: l = " << l_ << ", which is < 3");
    c_ = 2, s_ = 13 + a_[1];
  }
  else if (sn == 0x0e)
  {
    A(l_ >= 3, "η control size OOB: l = " << l_ << ", which is < 3");
    c_ = 3, s_ = 269 + (Sc<u16>(a_[1]) << 8 | a_[2]);
  }
  else
  {
    A(l_ >= 5, "η control size OOB: l = " << l_ << ", which is < 5");
    c_ = 5;
    s_ = 65805 + (  Sc<u32>(a_[1]) << 24
                  | Sc<u32>(a_[2]) << 16
                  | Sc<u32>(a_[3]) << 8
                  | a_[4]);
  }

  // NOTE: this looks weird, but we're making sure that we aren't
  // overflowing unsigned int math so the l_ check is valid.
  A(s_ + c_ > s_, "η size overflow");
  A(s_ + c_ <= l_,
    "η size overflows container: s_ = " << s_ <<
    ", c_ = " << Sc<int>(c_) <<
    ", l_ = " << l_ <<
    ", cb = " << Sc<int>(*a_));
}


static O &yone(O &s, ηi i)
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
