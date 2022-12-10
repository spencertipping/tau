#ifndef τπ0pgm_h
#define τπ0pgm_h


#include "types.hh"
#include "pi0-types.hh"
#include "pi0-abi.hh"

#include "begin.hh"

namespace τ
{


struct π0pgm
{
  π0abi const &a;  // ABI targeted by this program
  B            q;  // quoted statics
  V<π0b>       p;  // bytecodes
  π0bi         e;  // entry point

  π0pgm(π0pgm &&p_) : a(p_.a), q(std::move(p_.q)), p(std::move(p_.p)), e(p_.e) {}
  π0pgm(π0abi const &a_, Bc &q_, V<π0b> const &p_, π0bi e_)
    : a(a_), q(q_), p(p_), e(e_) {}
};


#if τdebug_iostream
O &operator<<(O &s, π0pgm const &p)
{
  s << "π₀p ABI=" << p.a.v() << " |q|=" << p.q.size() << std::endl;
  uN i = 0;
  for (let &[f, x] : p.p)
    s << (i == p.e ? "> " : "  ") << i << "\t" << p.a.n[f] << "\t" << x << std::endl,
      i++;
  return s;
}
#endif


}

#include "end.hh"


#endif
