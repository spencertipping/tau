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


}

#include "end.hh"
