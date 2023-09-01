#include <cppcodec/cppcodec/hex_lower.hpp>
#include <cppcodec/cppcodec/base64_rfc4648.hpp>

#include "types.hh"
#include "begin.hh"

namespace τ
{


Tt static T from_vec(Vc<unsigned char> &v)
{
  T r;
  A(v.size() == r.size(), "from_vec<T>(): " << v.size() << " != " << r.size());
  std::copy(v.begin(), v.end(), r.begin());
  return r;
}


St h_hex(h224 const &h) { return cppcodec::hex_lower::encode(h.data(), h.size()); }
St h_hex(h256 const &h) { return cppcodec::hex_lower::encode(h.data(), h.size()); }
St h_hex(h384 const &h) { return cppcodec::hex_lower::encode(h.data(), h.size()); }
St h_hex(h512 const &h) { return cppcodec::hex_lower::encode(h.data(), h.size()); }

St h_b64(h224 const &h) { return cppcodec::base64_rfc4648::encode(h.data(), h.size()); }
St h_b64(h256 const &h) { return cppcodec::base64_rfc4648::encode(h.data(), h.size()); }
St h_b64(h384 const &h) { return cppcodec::base64_rfc4648::encode(h.data(), h.size()); }
St h_b64(h512 const &h) { return cppcodec::base64_rfc4648::encode(h.data(), h.size()); }


h224 h224_hex(Stc &s) { return from_vec<h224>(cppcodec::hex_lower::decode(s)); }
h256 h256_hex(Stc &s) { return from_vec<h256>(cppcodec::hex_lower::decode(s)); }
h384 h384_hex(Stc &s) { return from_vec<h384>(cppcodec::hex_lower::decode(s)); }
h512 h512_hex(Stc &s) { return from_vec<h512>(cppcodec::hex_lower::decode(s)); }

h224 h224_b64(Stc &s) { return from_vec<h224>(cppcodec::base64_rfc4648::decode(s)); }
h256 h256_b64(Stc &s) { return from_vec<h256>(cppcodec::base64_rfc4648::decode(s)); }
h384 h384_b64(Stc &s) { return from_vec<h384>(cppcodec::base64_rfc4648::decode(s)); }
h512 h512_b64(Stc &s) { return from_vec<h512>(cppcodec::base64_rfc4648::decode(s)); }


Tt T operator^(T const &a, T const &b)
{
  T r;
  for (uN i = 0; i < r.size(); ++i) r[i] = a[i] ^ b[i];
  return r;
}

template h224 operator^<h224>(h224c&, h224c&);
template h256 operator^<h256>(h256c&, h256c&);
template h384 operator^<h384>(h384c&, h384c&);
template h512 operator^<h512>(h512c&, h512c&);


O &operator<<(O &s, byte_suffix b)
{
  switch (b)
  {
  case byte_suffix::one:  return s << "B";
  case byte_suffix::kilo: return s << "KB";
  case byte_suffix::kibi: return s << "KiB";
  case byte_suffix::mega: return s << "MB";
  case byte_suffix::mebi: return s << "MiB";
  case byte_suffix::giga: return s << "GB";
  case byte_suffix::gibi: return s << "GiB";
  case byte_suffix::tera: return s << "TB";
  case byte_suffix::tebi: return s << "TiB";
  case byte_suffix::peta: return s << "PB";
  case byte_suffix::pebi: return s << "PiB";
  case byte_suffix::exa:  return s << "EB";
  case byte_suffix::exbi: return s << "EiB";
  default:                return s << "*" << u64(b);
  }
}


O &operator<<(O &s, bytes b)
{
  if (b.n < 1ull << 10) return s << b.n << byte_suffix::one;
  if (b.n < 1ull << 20) return s << b.n / 1024 << byte_suffix::kibi;
  if (b.n < 1ull << 30) return s << b.n / (1024ull * 1024) << byte_suffix::mebi;
  if (b.n < 1ull << 40) return s << b.n / (1024ull * 1024 * 1024) << byte_suffix::gibi;
  if (b.n < 1ull << 50) return s << b.n / (1024ull * 1024 * 1024 * 1024) << byte_suffix::tebi;
  if (b.n < 1ull << 60) return s << b.n / (1024ull * 1024 * 1024 * 1024 * 1024) << byte_suffix::pebi;
  return s << b.n / (1024ull * 1024 * 1024 * 1024 * 1024 * 1024) << byte_suffix::exbi;
}


}

#include "end.hh"
