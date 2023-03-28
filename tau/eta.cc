#include "eta.hh"
#include "begin.hh"

namespace τ
{


bool ηo::reserve(uN l)
{
  if (o_.expired())
  {
    b_ = {Sc<u8*>(nullptr), 0};
    return false;
  }

  if (s_ + l > b_.size_bytes())
  {
    // Abort this allocation and create a new one at twice the current size
    // or s_ + l, whichever is larger.
    u8 *b = new u8[s_];
    memcpy(b, b_.data(), s_);
    p_->abort();
    let s = std::max(s_ + l, Sc<uN>(b_.size_bytes() << 1));

    // Complete the copy only if we actually have memory. ξ can be deallocated
    // during the iptr() call, in which case we'll get an empty span back.
    if (!(b_ = p_->iptr(s)).empty()) memcpy(b_.data(), b, s_);
    delete[] b;
  }

  return !b_.empty();
}


O &operator<<(O &s, ηtype t)
{
  return s << "t" << Sc<int>(t);
}

O &operator<<(O &s, ηatom a)
{
  return s << "a" << Sc<int>(a);
}

O &operator<<(O &s, ηsig x)
{
  switch (x)
  {
  case ηsig::α: return s << "α";
  case ηsig::κ: return s << "κ";
  case ηsig::ι: return s << "ι";
  case ηsig::τ: return s << "τ";
  case ηsig::ω: return s << "ω";
  case ηsig::Ω: return s << "Ω";
  case ηsig::ρ: return s << "ρ";
  case ηsig::θ: return s << "θ";
  default:      return s << "s" << Sc<int>(x);
  }
}


O &operator<<(O &s, ηi const &i)
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
  case ηtype::η:
  {
    ηi x = i.η();
    s << "(" << x;
    while (x.has_next()) s << " " << (x = x.next());
    return s << ")";
  }
  default:
    return s << "ηi[t=" << Sc<int>(i.t()) << ", s=" << i.size() << "]";
  }
}


}

#include "end.hh"
