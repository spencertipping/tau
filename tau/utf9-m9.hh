#ifndef τutf9_m9_h
#define τutf9_m9_h


#include "types.hh"
#include "utf9-i9.hh"
#include "utf9-o9.hh"


#include "begin.hh"

namespace τ
{


struct m9  // memory-resident immediate UTF9 value
{
  ζp b;

  template<o9__ T> explicit m9(T x) {                b = std::malloc(x.size()); x.write(b); }
  template<o9n_ T> explicit m9(T x) { let o = o9(x); b = std::malloc(o.size()); o.write(b); }
  ~m9() { std::free(b); }

  operator i9() { return i9{b}; }
};


#if τdebug_iostream
O &operator<<(O &s, m9 const &m)
{
  return s << Sc<i9>(m);
}
#endif


}

#include "end.hh"


#endif
