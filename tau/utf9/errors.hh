#ifndef tau_utf9_errors_h
#define tau_utf9_errors_h


#include <iostream>
#include <stdexcept>


#include "../types.hh"

#include "error-proto.hh"
#include "ibuf.hh"
#include "tval.hh"
#include "val.hh"

#include "../module/begin.hh"


namespace tau::utf9
{


struct utf9_error : public std::exception
{
  std::string const m;
  utf9_error(std::string const &m_) : m(m_) {}
  virtual std::ostream &operator>>(std::ostream&) const = 0;

  static std::string hexify(u8 b)
    { char r[2] = {"0123456789abcdef"[b >> 4], "0123456789abcdef"[b & 15]};
      return std::string(r, 2); }
};

struct internal_error : virtual public utf9_error
{
  internal_error(std::string const &m_) : utf9_error(m_) {}
  std::ostream &operator>>(std::ostream &s) const
    { return s << "internal_error " << m; }
};

struct badbyte_error : virtual public utf9_error
{
  u8 byte;
  badbyte_error(std::string const &m_, u8 b_) : utf9_error(m_), byte(b_) {}
  std::ostream &operator>>(std::ostream &s) const
    { return s << "badbyte_error " << m << ": " << static_cast<int>(byte); }
};

struct decoding_error : virtual public utf9_error
{
  ibuf const b;
  uNc  i;
  decoding_error(std::string const &m_, ibuf const &b_, uN i_)
    : utf9_error(m_), b(b_), i(i_) {}
  std::ostream &operator>>(std::ostream&) const;
};

struct toperation_error : virtual public utf9_error
{
  tval const t;
  toperation_error(std::string const &m_, tval const &t_) : utf9_error(m_), t(t_) {}
  std::ostream &operator>>(std::ostream&) const;
};

struct voperation_error : virtual public utf9_error
{
  val const v;
  voperation_error(std::string const &m_, val const &v_) : utf9_error(m_), v(v_) {}
  std::ostream &operator>>(std::ostream&) const;
};

struct binop_error : virtual public utf9_error
{
  val const a;
  val const b;
  binop_error(std::string const &m_, val const &a_, val const &b_)
    : utf9_error(m_), a(a_), b(b_) {}
  std::ostream &operator>>(std::ostream&) const;
};

struct encoding_error : virtual public utf9_error
{
  tval const t;
  val  const v;
  encoding_error(std::string const &m_, tval const &t_, val const &v_)
    : utf9_error(m_), t(t_), v(v_) {}
  std::ostream &operator>>(std::ostream&) const;
};


inline std::ostream &operator<<(std::ostream &s, utf9_error const &e) { return e >> s; }


template<class T> inline T throw_internal_error(std::string const &m)                              { throw internal_error(m); }
template<class T> inline T throw_badbyte_error (std::string const &m, u8 b)                        { throw badbyte_error(m, b); }
template<class T> inline T throw_decoding_error(std::string const &m, ibuf const &b, uN i)         { throw decoding_error(m, b, i); }
template<class T> inline T throw_top_error     (std::string const &m, tval const &t)               { throw toperation_error(m, t); }
template<class T> inline T throw_vop_error     (std::string const &m, val const &v)                { throw voperation_error(m, v); }
template<class T> inline T throw_binop_error   (std::string const &m, val const &a, val const &b)  { throw binop_error(m, a, b); }
template<class T> inline T throw_encoding_error(std::string const &m, tval const &t, val const &v) { throw encoding_error(m, t, v); }


}


#include "../module/end.hh"

#endif
