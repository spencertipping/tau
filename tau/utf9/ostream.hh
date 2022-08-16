#ifndef tau_utf9_ostream_h
#define tau_utf9_ostream_h


#include <iostream>
#include <ios>


#include "../types.hh"
#include "../util/ostream.hh"

#include "error-proto.hh"
#include "errors.hh"
#include "primitive.hh"
#include "tval.hh"
#include "typecode.hh"
#include "val.hh"


#include "../module/begin.hh"


namespace tau::utf9
{

using tau::util::hexout;


std::ostream &operator<<(std::ostream &s, obuf const &o)
{
  return s.write(Rc<char*>(o.b), o.i);
}


#if tau_debug_iostream
std::ostream &operator<<(std::ostream &s, hash  const &h) { return hexout(s << 'h', h.h); }
std::ostream &operator<<(std::ostream &s, pidfd const &p) { return s << "[p=" << p.pid << ",fd=" << p.fd << "]"; }


std::ostream &operator<<(std::ostream &s, val_type t)
{
  switch (t)
  {
  case UINT8:    return s << "u8";
  case UINT16:   return s << "u16";
  case UINT32:   return s << "u32";
  case UINT64:   return s << "u64";
  case UINT:     return s << "uint";
  case INT8:     return s << "i8";
  case INT16:    return s << "i16";
  case INT32:    return s << "i32";
  case INT64:    return s << "i64";
  case INT:      return s << "int";
  case FLOAT32:  return s << "f32";
  case FLOAT64:  return s << "f64";
  case SYMBOL:   return s << "sym";
  case PIDFD:    return s << "pidfd";
  case BOOL:     return s << "bool";
  case NULLTYPE: return s << "null";
  case GREEK:    return s << "greek";

  case UTF8:     return s << "utf8";
  case BYTES:    return s << "bytes";
  case TUPLE:    return s << "tuple";
  case ARRAY:    return s << "array";
  case LIST:     return s << "list";
  case SET:      return s << "set";
  case MAP:      return s << "map";

  case BOGUS:    return s << "bogus";
  case NONE:     return s << "none";
  default:       return s << "??? " << Sc<u8>(t);
  }
}


std::ostream &operator<<(std::ostream &s, tval const &t)
{
  switch (t.type())
  {
  case UINT8:   return s << "u8";
  case UINT16:  return s << "u16";
  case UINT32:  return s << "u32";
  case UINT64:  return s << "u64";
  case UINT:    return s << "uint";
  case INT8:    return s << "i8";
  case INT16:   return s << "i16";
  case INT32:   return s << "i32";
  case INT64:   return s << "i64";
  case INT:     return s << "int";
  case FLOAT32: return s << "f32";
  case FLOAT64: return s << "f64";

  case SYMBOL:  return s << "sym";
  case PIDFD:   return s << "pidfd";

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

  default: return throw_badbyte_error<std::ostream&>("ns typecode", t.type());
  }
}


std::ostream &operator<<(std::ostream &s, val const &v)
{
  switch (v.type())
  {
  case UINT: case UINT8: case UINT16: case UINT32: case UINT64: return s << Sc<u64>(v);
  case INT:  case INT8:  case INT16:  case INT32:  case INT64:  return s << Sc<i64>(v);
  case FLOAT32: return s << Sc<f32>(v);
  case FLOAT64: return s << Sc<f64>(v);
  case SYMBOL:  return s << Sc<sha256>(v);
  case PIDFD:   return s << Sc<pidfd>(v);

  case GREEK:
  {
    let g = Sc<greek>(v);
    switch(g.l)
    {
    case greek::Α: return s << "α";
    case greek::Ι: return s << "ι";
    case greek::Κ: return s << "κ";
    case greek::Ρ: return s << "ρ(" << g.v << ")";
    case greek::Θ: return s << "θ(" << Sc<f64>(g.v) / Sc<f64>(Nl<u32>::max()) << ")";
    case greek::Τ: return s << "τ";
    case greek::Ω: return s << "ω";
    default: return throw_badbyte_error<std::ostream&>("greek<<", g.l);
    }
  }

  case UTF8:  return s << "u8[" << Sc<std::string_view>(v) << "]";
  case BYTES: return s << "b["  << Sc<std::string_view>(v) << "]";  // FIXME: should be hex

  case TUPLE:
  case LIST:
  case SET:
  case MAP:
  { let t = v.type();
    s << (t == TUPLE ? "(" : t == LIST ? "[" : "{");
    bool first = true;
    for (let &x : v)
    { if (first) first = false;
      else       s << ", ";
      if (t == MAP && x.type() == TUPLE && x.len() == 2) s << x[0] << ": " << x[1];
      else                                               s << x; }
    return s << (t == TUPLE ? ")" : t == LIST ? "]" : "}"); }

  case ARRAY:
  { s << "array<" << v.atype() << ">[" << v.len() << "][";
    bool first = true;
    for (let &x : v)
    { if (first) first = false;
      else       s << ", ";
      s << x; }
    return s << "]"; }

  case NONE:  return s << "none";
  case BOGUS: return s << "bogus(" << v.tag << ", " << v.i << ")";
  default:    return s << "??? t=" << v.type();
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
  return s << "toperation_error(" << m << ": " << t << ")";
}

std::ostream &voperation_error::operator>>(std::ostream &s) const
{
  return s << "voperation_error(" << m << ": " << v << ")";
}

std::ostream &binop_error::operator>>(std::ostream &s) const
{
  return s << "binop_error(" << m << ": " << a << ", " << b << ")";
}

std::ostream &encoding_error::operator>>(std::ostream &s) const
{
  return s << "encoding_error(" << m << ": " << t << " " << v << ")";
}
#elif tau_debug_nop
std::ostream &operator<<(std::ostream &s, hash  const &) { return s; }
std::ostream &operator<<(std::ostream &s, pidfd const &) { return s; }
std::ostream &operator<<(std::ostream &s, val_type t)    { return s; }
std::ostream &operator<<(std::ostream &s, tval const &t) { return s; }
std::ostream &operator<<(std::ostream &s, val const &v)  { return s; }
#endif


}


#include "../module/end.hh"

#endif
