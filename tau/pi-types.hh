#ifndef τπtypes_h
#define τπtypes_h


#include "types.hh"
#include "utf9.hh"

#include "begin.hh"

namespace τ
{


struct πi;
struct πF;
struct πh;

typedef uN             πr;   // zero for success
typedef F<πr(πi&, uN)> πf;   // instruction function
typedef V<πf>          πfs;  // function resolution table
typedef m9             πt;   // utf9-encoded type


struct πF
{
  πt   q;   // query terms (a set)
  πt   t;   // function type
  f64  η0;  // prior entropy
  πf   f;
};



#if τdebug_iostream
O &operator<<(O &s, πF const &f)
{
  return s << f.q << "[" << f.η0 << "] ∷ " << f.t;
}
#endif


}

#include "end.hh"


#endif
