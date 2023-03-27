#include "eta.hh"
#include "begin.hh"

namespace τ
{


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
