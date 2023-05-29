#include "eta.hh"
#include "begin.hh"

namespace τ
{


O &operator<<(O &s, ηts ts)
{
  s << "ηts[";
  for (uN i = 0; i < ts.size(); ++i)
    s << (i ? " " : "") << ts[i];
  return s << "]";
}


O &operator<<(O &s, ηtype t)
{
  switch (t)
  {
  case ηtype::sig:      return s << "sig";
  case ηtype::n_int:    return s << "i64";
  case ηtype::n_float:  return s << "f64";
  case ηtype::string:   return s << "str";
  case ηtype::atom:     return s << "atom";
  case ηtype::name:     return s << "name";
  case ηtype::int8s:    return s << "i8s";
  case ηtype::int16s:   return s << "i16s";
  case ηtype::int32s:   return s << "i32s";
  case ηtype::int64s:   return s << "i64s";
  case ηtype::float32s: return s << "f32s";
  case ηtype::float64s: return s << "f64s";
  case ηtype::η:        return s << "η";
  default:              return s << "t" << Sc<int>(t);
  }
}

O &operator<<(O &s, ηatom a)
{
  switch (a)
  {
  case ηatom::ηfalse: return s << "false";
  case ηatom::ηtrue:  return s << "true";
  case ηatom::ηnull:  return s << "null";
  default:            return s << "a" << Sc<int>(a);
  }
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


}

#include "end.hh"
