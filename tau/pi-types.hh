#ifndef τπtypes_h
#define τπtypes_h


#include "types.hh"
#include "utf9-types.hh"
#include "utf9.hh"

#include "begin.hh"

namespace τ
{


struct πi;
struct πF;
struct πh;
struct πs;
struct πp;

typedef uN             πr;   // zero for success
typedef F<πr(πi&, uN)> πf;   // instruction function
typedef V<πF*>         πfs;  // function resolution table
typedef m9             πt;   // utf9-encoded type
typedef u32            πb;   // bytecode instruction


struct πF
{
  u9_symbol n;
  πt        q;   // query terms (a set)
  πt        t;   // function type
  uN        a;   // bytecode arity
  f64       η0;  // prior entropy
  πf        f;

  πF(u9_symbol s, uN a_, πf f_) : n(s),
                                  q(m9("TODO")), t(m9("TODO")),
                                  a(a_), f(f_) {}

  πF(uN a_, πf f_) : n(u9_symbol::gensym()),
                     q(m9("TODO")), t(m9("TODO")),
                     a(a_), f(f_) {}

  πr operator()(πi& i, uN o) const { return f(i, o); }
};


// FIXME: fs should be global; if it's specific to a program, then we
// lose bytecode portability

struct πp
{
  πfs   fs;
  V<πb> b;   // bytecodes
  uN    ng;  // number of globals
  uN    p0;  // initial instruction (as index into b)

  πb operator[](uN n)        const { return b[n]; }
  πr operator()(πi &i, uN n) const { return (*fs[n])(i, n); }
};


#if τdebug_iostream
O &operator<<(O &s, πF const &f)
{
  return s << f.n;
}

O &operator<<(O &s, πp const &p)
{
  s << "πp ng=" << p.ng << ", p₀=" << p.p0 << std::endl;
  for (uN i = 0; i < p.b.size();)
  { let &f = *p.fs[p[i]];
    s << "  " << i << "\t" << f << "\t";
    for (uN j = 0; j < f.a; ++j) s << p[i + j] << " ";
    s << std::endl;
    i += f.a; }
  return s;
}
#endif


}

#include "end.hh"


#endif
