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


St ηi::inspect() const
{
  Ss s;
  s << *this;
  return s.str();
}


void ηi::decode_cb()
{
  ηicb_r r;

  uN orig_s_ = 0;
  uN orig_c_ = 0;

  if (!l_) { r = ηicb_r::no_ctrl; goto err; }

  {
    let sn = *a_ & 0x0f;

    if      (sn <= 0x0c) c_ = 1, s_ = sn;
    else if (sn == 0x0d)
    {
      if (l_ < 2) { r = ηicb_r::short_len1; goto err; }
      c_ = 2, s_ = 13 + a_[1];
    }
    else if (sn == 0x0e)
    {
      if (l_ < 3) { r = ηicb_r::short_len2; goto err; }
      c_ = 3, s_ = 269 + (u16(a_[1]) << 8 | a_[2]);
    }
    else
    {
      if (l_ < 5) { r = ηicb_r::short_len4; goto err; }
      c_ = 5;
      s_ = 65805 + ( u32(a_[1]) << 24
                   | u32(a_[2]) << 16
                   | u32(a_[3]) << 8
                   |     a_[4]);
    }
  }

  // NOTE: this looks weird, but we're making sure that we aren't
  // overflowing unsigned int math so the l check is valid.
  if (s_ + c_ <= s_) { r = ηicb_r::int_overflow;  goto err; }
  if (s_ + c_ > l_)  { r = ηicb_r::size_overflow; goto err; }
  r = ηicb_r::ok;
  goto done;

err:
  orig_s_ = s_;
  orig_c_ = c_;

  s_ = c_ = 0;

done:
  A(r == ηicb_r::ok || r == ηicb_r::no_ctrl,
    "ηi bounds error: " << r << " at " << (void*) a_ << "+" << l_ <<
    ": (s=" << orig_s_ << ", c=" << orig_c_ << ", l=" << l_ << "); " <<
    "e = " << r << "; " <<
    "a[0] = " << std::hex << (l_ > 0 ? *(uint8_t*) a_    : -1) << std::dec << " " <<
    "a[1] = " << std::hex << (l_ > 1 ? *(uint8_t*)(a_+1) : -1) << std::dec << " " <<
    "a[2] = " << std::hex << (l_ > 2 ? *(uint8_t*)(a_+2) : -1) << std::dec << " " <<
    "a[3] = " << std::hex << (l_ > 3 ? *(uint8_t*)(a_+3) : -1) << std::dec << " " <<
    "a[4] = " << std::hex << (l_ > 4 ? *(uint8_t*)(a_+4) : -1) << std::dec << " " <<
    "a[5] = " << std::hex << (l_ > 5 ? *(uint8_t*)(a_+5) : -1) << std::dec << " " <<
    "a[6] = " << std::hex << (l_ > 6 ? *(uint8_t*)(a_+6) : -1) << std::dec << " " <<
    "a[7] = " << std::hex << (l_ > 7 ? *(uint8_t*)(a_+7) : -1) << std::dec << " " <<
    "a[8] = " << std::hex << (l_ > 8 ? *(uint8_t*)(a_+8) : -1) << std::dec << " " <<
    "a[9] = " << std::hex << (l_ > 9 ? *(uint8_t*)(a_+9) : -1) << std::dec << " ");

  // Segfault if we try to access data in an empty η
  if (r == ηicb_r::no_ctrl) a_ = nullptr;
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


static O &yone(O &s, ηic &i)
{
  switch (i.t())
  {
  case ηtype::sig:      return s << i.sig();
  case ηtype::n_int:    return s << i.i();
  case ηtype::n_float:  return s << i.f();
  case ηtype::string:   return s << "\"" << i.s() << "\"";
  case ηtype::atom:     return s << i.a();
  case ηtype::name:     return s << i.n().x << ":";
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
  while (i.has_next()) yone(s << " ", ++i);
  return s;
}


}

#include "end.hh"
