#ifndef τξio_h
#define τξio_h


#include "xi.hh"
#include "eta.hh"
#include "begin.hh"

namespace τ
{


struct ξio;  // a Sp<ξ> container that tracks when read/write are claimed
struct ξi;   // a ξ reader, strongly-referenced
struct ξo;   // a ξ writer, weakly-referenced


struct ξi
{
  ξi(Sp<ξ> x_) : x(x_) {}

  ξ::it begin() { return x->begin(); }
  ξ::it end()   { return x->end(); }

  void     close()       { x.reset(); }
  bool       eof() const {            return x->eof(); }
  η0i operator *() const {            return **x; }
  ξi &operator++()       { x->next(); return *this; }

protected:
  Sp<ξ> x;
};


struct ξo
{
  ξo(Sp<ξ> x_) : x(x_) {}

  void       close()         { if (let y = x.lock()) y->close(); }
  bool operator<< (η0o const &z) { let y = x.lock(); return y && *y << z; }
  bool operator<<=(η0o const &z) { let y = x.lock(); return y && (*y <<= z); }

protected:
  Wp<ξ> x;
};


struct ξio
{
  ξio(Sp<ξ> x_) : x(x_), i_(false), o_(false) {}

  ξi i() { A(!i_, "ξi already claimed"); i_ = true; return ξi{x}; }
  ξo o() { A(!o_, "ξi already claimed"); o_ = true; return ξo{x}; }

protected:
  Sp<ξ> x;
  bool  i_;  // has ξi been claimed?
  bool  o_;  // has ξo been claimed?
};


}

#include "end.hh"


#endif
