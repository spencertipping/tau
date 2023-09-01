#include "eta.hh"
#include "begin.hh"

namespace τ
{


ηtype ηlub(ηtype a, ηtype b)
{
  sletc sn  = ηts({ηtype::n_int, ηtype::n_float});
  sletc ss  = ηts({ηtype::n_int, ηtype::n_float, ηtype::atom, ηtype::string});
  sletc svi = ηts({ηtype::int8s, ηtype::int16s, ηtype::int32s, ηtype::int64s});
  sletc svf = ηts({ηtype::float32s, ηtype::float64s});
  sletc sv  = svi | svf;

  if (a == b) return a;
  if (sn[a] && sn[b]) return ηtype::n_float;
  if (ss[a] && ss[b]) return ηtype::string;
  if (sv[a] && sv[b]) return std::max(a, b);
  if (sv[a] && sn[b]) return a;  // right broadcast
  if (sn[a] && sv[b]) return b;  // left broadcast
  return ηtype::invalid;
}


O &operator<<(O &s, ηts ts)
{
  s << "ηts[";
  bool first = true;
  for (uN i = 0; i < 16; ++i)
    if (ts.x & (1 << i))
    {
      if (first) first = false;
      else s << " ";
      s << ηtype(i);
    }
  return s << "]";
}


O &operator<<(O &s, ηtl ts)
{
  s << "ηtl[";
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
  case ηtype::binary:   return s << "binary";
  case ηtype::int8s:    return s << "i8s";
  case ηtype::int16s:   return s << "i16s";
  case ηtype::int32s:   return s << "i32s";
  case ηtype::int64s:   return s << "i64s";
  case ηtype::float32s: return s << "f32s";
  case ηtype::float64s: return s << "f64s";
  case ηtype::ext:      return s << "ext";
  case ηtype::η:        return s << "η";
  default:              return s << "t" << int(t);
  }
}

O &operator<<(O &s, ηatom a)
{
  switch (a)
  {
  case ηatom::ηfalse: return s << "false";
  case ηatom::ηtrue:  return s << "true";
  case ηatom::ηnull:  return s << "null";
  default:            return s << "a" << int(a);
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
  default:      return s << "s" << int(x);
  }
}


O &operator<<(O &s, ηname const &n)
{
  return s << "'" << n.x;
}


}

#include "end.hh"
