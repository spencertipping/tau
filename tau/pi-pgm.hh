#ifndef τπpgm_h
#define τπpgm_h


#include "types.hh"
#include "pi-types.hh"
#include "pi-abi.hh"

#include "begin.hh"

namespace τ
{


struct πpgm
{
  πabi const &a;  // ABI targeted by this program
  B           q;  // quoted statics
  V<πb>       p;  // bytecodes
  πbi         e;  // entry point

  πpgm(πpgm &&p_) : a(p_.a), q(std::move(p_.q)), p(std::move(p_.p)), e(p_.e) {}
  πpgm(πabi const &a_, Bc &q_, V<πb> const &p_, πbi e_)
    : a(a_), q(q_), p(p_), e(e_) {}
};


#if τdebug_iostream
O &operator<<(O &s, πpgm const &p)
{
  s << "π₀p ABI=" << p.a.v() << " |q|=" << p.q.size() << std::endl;
  uN i = 0;
  for (let &[f, x] : p.p)
  {
    s << (i == p.e ? "> " : "  ") << i << "\t" << p.a.n[f] << "\t" << x << std::endl;
    ++i;
  }
  return s;
}
#endif


}

#include "end.hh"


#endif
