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
    c_ = 3, s_ = 269 + (Sc<u16>(a_[0]) << 8 | a_[1]);
  }
  else
  {
    A(l_ >= 5, "η control size OOB: l = " << l_ << ", which is < 5");
    c_ = 5;
    s_ = 65805 + (  Sc<u32>(a_[0]) << 24
                    | Sc<u32>(a_[1]) << 16
                    | Sc<u32>(a_[2]) << 8
                    | a_[3]);
  }

  A(s_ + c_ > s_, "η size overflow");
  A(s_ + c_ <= l_,
    "η size overflows container: s_ = " << s_ << ", c_ = " << c_ << ", l_ = " << l_);
}


}

#include "end.hh"
