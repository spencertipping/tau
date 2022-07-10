#pragma once


#include <iostream>

#include "primitive.hh"
#include "tval.hh"
#include "typecode.hh"
#include "val.hh"


namespace tau::utf9
{

#define let auto const


namespace  // val/tval << operators
{

std::ostream &operator<<(std::ostream &s, sym   const &y) { return s << 's' << reinterpret_cast<void*>(y.h); }
std::ostream &operator<<(std::ostream &s, hash  const &h) { return s << 'h' << reinterpret_cast<void*>(h.h); }
std::ostream &operator<<(std::ostream &s, pidfd const &p) { return s << "[p=" << p.pid << ",fd=" << p.fd << "]"; }

std::ostream &operator<<(std::ostream &s, val_type t)
{
  switch (t)
  {
  case UINT:     return s << "uint";
  case INT:      return s << "int";
  case FLOAT32:  return s << "f32";
  case FLOAT64:  return s << "f64";
  case SYMBOL:   return s << "sym";
  case PIDFD:    return s << "pidfd";
  case BOOL:     return s << "bool";
  case NULLTYPE: return s << "null";

  case Α:        return s << "α";
  case Ω:        return s << "ω";
  case Ι:        return s << "ι";
  case Κ:        return s << "κ";
  case Τ:        return s << "τ";
  case Ρ:        return s << "ρ";
  case Θ:        return s << "θ";

  case UTF8:     return s << "utf8";
  case BYTES:    return s << "bytes";
  case TUPLE:    return s << "tuple";
  case ARRAY:    return s << "array";
  case INDEX:    return s << "index";

  case BOGUS:    return s << "bogus";
  case NONE:     return s << "none";
  default:       return s << "??? " << static_cast<uint8_t>(t);
  }
}

std::ostream &operator<<(std::ostream &s, tval const &t)
{
  switch (t.type())
  {
  case UINT:    return s << "u" << t.vsize() * 8;
  case INT:     return s << "i" << t.vsize() * 8;
  case FLOAT32:
  case FLOAT64: return s << "f" << t.vsize() * 8;
  case SYMBOL:  return s << "s";
  case PIDFD:   return s << "p";

  case UTF8:    return s << "u[" << t.vsize() << "]";
  case BYTES:   return s << "b[" << t.vsize() << "]";
  case TUPLE:
  { bool first = true;
    s << "(";
    for (let &v : t)
    { if (first) first = false;
      else       s << ", ";
      s << v; }
    return s << ")"; }

  case ARRAY: return s << t.atype() << "[" << t.len() << "]";

  default: throw internal_error("ns typecode");
  }
}

std::ostream &operator<<(std::ostream &s, val const &v)
{
  switch (v.type())
  {
  case UINT:    return s << static_cast<uint64_t>(v);
  case INT:     return s << static_cast<int64_t>(v);
  case FLOAT32: return s << static_cast<float>(v);
  case FLOAT64: return s << static_cast<double>(v);
  case SYMBOL:  return s << static_cast<sym>(v);
  case PIDFD:   return s << static_cast<pidfd>(v);

  case Α: return s << "α";
  case Ω: return s << "ω";
  case Ι: return s << "ι";
  case Κ: return s << "κ";
  case Τ: return s << "τ";
  case Ρ: return s << "ρ(" << static_cast<uint64_t>(v) << ")";
  case Θ: return s << "θ(" << static_cast<double>(v) << ")";

  case UTF8:  return s << "u8[" << static_cast<std::string_view>(v) << "]";
  case BYTES: return s << "b["  << static_cast<std::string_view>(v) << "]";

  case TUPLE:
  { s << "(";
    bool first = true;
    for (let &x : v)
    { if (first) first = false;
      else       s << ", ";
      s << x; }
    return s << ")"; }

  case ARRAY:
  { s << "array<" << v.atype() << ">[" << v.len() << "][";
    bool first = true;
    for (uint64_t i = 0; i < v.len(); ++i)
    { if (first) first = false;
      else       s << ", ";
      s << val(v.atype(), *v.b, v.asub(i)); }
    return s << "]"; }

  case INDEX: return s << "index[]";

  case NONE:  return s << "none";
  case BOGUS: return s << "bogus";
  default:    return s << "???";
  }
}

std::ostream &operator<<(std::ostream &s, obuf const &o)
{
  return s.write(reinterpret_cast<char*>(o.b), o.i);
}

}


std::ostream &decoding_error::operator>>(std::ostream &s) const
{
  s << "decoding_error " << m << std::endl;
  s << "  position = " << i << ", len = " << b.size() << std::endl;
  s << "  bytes = ";
  for (int j = 0; j < 8; ++j) s << hexify(b.xs[i + j]) << " ";
  return s << std::endl;
}

std::ostream &toperation_error::operator>>(std::ostream &s) const
{
  return s << "toperation_error" << m << " " << t;
}

std::ostream &voperation_error::operator>>(std::ostream &s) const
{
  return s << "voperation_error" << m << " " << v;
}

std::ostream &binop_error::operator>>(std::ostream &s) const
{
  return s << "binop_error" << m << "(" << a << ", " << b << ")";
}

std::ostream &encoding_error::operator>>(std::ostream &s) const
{
  return s << "encoding_error" << m << " " << t << " " << v;
}


#undef let

}
