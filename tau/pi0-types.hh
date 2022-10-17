#ifndef τπ0types_h
#define τπ0types_h


#include "types.hh"
#include "utf9-types.hh"
#include "utf9.hh"

#include "pi0-gc-begin.hh"

namespace τ
{


struct π0int;
struct π0asm;


// TODO: add uN/void* arg here and make this a bare function pointer
typedef F<void(π0int&)> π0f;  // bytecode function


typedef uN π0hg;              // heap generation
typedef uN π0ha;              // heap address within generation

π0TG
struct π0r                    // π₀ generational heap reference
{ π0TS;
  sletc gn   = Sc<uN>(1) << Gb;
  sletc ghms = τwordsize - Gb;
  sletc ghm  = gn - 1 << ghms;
  sletc gam  = ~ghm;
  static_assert((ghm | gam) == -1);
  static_assert((ghm & gam) ==  0);

  uN ga;
  π0r()                 : ga(-1)             {}
  π0r(π0hg g, π0ha a_)  : ga(g << ghms | a_) {}
  explicit π0r(uN ga_)  : ga(ga_)            {}

  π0hg g() const { return ga >> ghms; }
  uN   a() const { return ga  & gam;  }
  bool ω() const { return ga == -1; }

  bool operator==(π0T(π0r) x) const { return ga == x.ga; }
  bool operator< (π0T(π0r) x) const { return ga <  x.ga; }
};


#if τdebug_iostream
π0TG
O &operator<<(O &s, π0T(π0r) r)
{
  return r.ω() ? s << "π₀r:ω" : s << "π₀r:" << r.g() << ":" << r.a();
}
#endif


}


π0TGn
struct std::hash<π0T(τ::π0r)>
{
  size_t operator()(π0T(τ::π0r) a) const
    { return std::hash<τ::uN>{}(a.ga); }
};


#include "pi0-gc-end.hh"


#endif
