#ifndef τπ0pgm_h
#define τπ0pgm_h


#include "types.hh"
#include "pi0-types.hh"
#include "pi0-abi.hh"

#include "pi0-begin.hh"

namespace τ
{


π0TGs π0p
{
  π0TS;
  π0T(π0abi) const &a;  // ABI targeted by this program
  B                 q;  // quoted statics
  V<π0b>            p;  // bytecodes

  π0p(π0T(π0p) &&p_) : a(p_.a), q(std::move(p_.q)), p(std::move(p_.p)) {}
  π0p(π0T(π0abi) const &a_, Bc &q_, V<π0b> const &p_) : a(a_), q(q_), p(p_) {}
};


#if τdebug_iostream
π0TG O &operator<<(O &s, π0T(π0p) const &p)
{
  s << "π₀p ABI=" << p.a.v() << " |q|=" << p.q.size() << std::endl;
  uN i = 0;
  for (let &[f, x] : p.p)
    s << "  " << i++ << "\t" << p.a.n[f] << "\t" << x << std::endl;
  return s;
}
#endif


}

#include "pi0-end.hh"


#endif
