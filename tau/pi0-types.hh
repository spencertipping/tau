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

typedef void(*π0f)(π0int&, uN);  // bytecode function


π0TGs π0h;                       // GC heap (all generations)
π0TGs π0hs;                      // GC heap space
π0TGs π0hv;                      // GC heap view
π0TGs π0ms;                      // GC mark set
π0TGs π0gs;                      // GC splice map

typedef uN π0hg;                 // heap generation
typedef uN π0ha;                 // heap address within generation

π0TGs π0r                        // π₀ generational heap reference
{
  π0TS;
  sletc gn   = Sc<uN>(1) << Gb;
  sletc ghms = τwordsize - Gb;
  sletc ghm  = gn - 1 << ghms;
  sletc gam  = ~ghm;
  static_assert((ghm | gam) == -1);
  static_assert((ghm & gam) ==  0);

  uN ga;
  π0r()                : ga(-1)             {}
  π0r(π0hg g, π0ha a_) : ga(g << ghms | a_) {}
  explicit π0r(uN ga_) : ga(ga_)            {}
  explicit π0r(i9 i)
  { let s = Sc<u9_scoped<u9_π, uN>>(i);
    A(s.t == u9_π::ref, "π₀r from non-ref " << i);
    ga = s.x; }

  π0hg g() const { return ga >> ghms; }
  uN   a() const { return ga  & gam;  }
  bool ω() const { return ga == -1; }

  operator bool() const { return !ω(); }

  bool operator==(π0R x) const { return ga == x.ga; }
  bool operator< (π0R x) const { return ga <  x.ga; }
  π0R  operator+ (iN  x) const { return π0R(g(), a() + x); }
  π0R  operator+ (uN  x) const { return *this + Sc<iN>(x); }
  iN   operator- (π0R x) const
  { A(g() == x.g(), "π0R- g mismatch: " << g() << " ≠ " << x.g());
    return a() - x.a(); }

};


#if τdebug_iostream
π0TG O &operator<<(O &s, π0T(π0r) r)
{
  return r.ω() ? s << "π₀r:ω" : s << "π₀r:" << r.g() << ":" << r.a();
}
#endif


}


π0TGn struct std::hash<π0T(τ::π0r)>
{
  size_t operator()(π0T(τ::π0r) a) const
    { return std::hash<τ::uN>{}(a.ga); }
};


#include "pi0-gc-end.hh"


#endif
