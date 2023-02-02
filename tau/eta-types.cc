#include "eta-types.hh"

#include "begin.hh"

namespace τ
{


O &operator<<(O &s, η0sig x)
{
  switch (x)
  {
  case η0sig::α: return s << "α";
  case η0sig::κ: return s << "κ";
  case η0sig::ι: return s << "ι";
  case η0sig::τ: return s << "τ";
  case η0sig::ω: return s << "ω";
  default:       return s << "sig" << Sc<uN>(x);
  }
}

O &operator<<(O &s, η0t t)
{
  switch (t)
  {
  case η0t::η0:       return s << "η₀";
  case η0t::signal:   return s << "sig";
  case η0t::symbol:   return s << "sym";
  case η0t::bytes:    return s << "b[]";
  case η0t::utf8:     return s << "utf8";
  case η0t::int_be:   return s << "int";
  case η0t::uint_be:  return s << "uint";
  case η0t::float_be: return s << "float";
  case η0t::boolean:  return s << "bool";
  case η0t::tuple:    return s << "()";
  case η0t::set:      return s << "set";
  case η0t::map:      return s << "map";
  case η0t::invalid:  return s << "invalid";
  default:            return s << "t" << Sc<uN>(t);
  }
}

O &operator<<(O &s, η0ts x)
{
  u64 m = x.m;
  s << "{";
  for (iN i = 0; m >> i; ++i)
  {
    if (m >> i & 1)
    {
      s << Sc<η0t>(i);
      if (m >> i + 1) s << ",";
    }
  }
  return s << "}";
}


}

#include "end.hh"
