#ifndef τπ0types_h
#define τπ0types_h


#include "types.hh"
#include "utf9-types.hh"
#include "utf9.hh"

#include "pi0-begin.hh"


namespace τ
{


π0TGs π0int;
π0TGs π0asm;
π0TGs π0abi;


typedef uN π0fi;  // function index
typedef uN π0fa;  // function argument
typedef uN π0av;  // ABI version

// Bytecode function: takes a single native-width argument whose meaning
// is determined by the function
π0TG using π0f = void(*)(π0T(π0int)&, π0fa);

// Bytecode instantiation: function index + arg
typedef P<π0fi, π0fa> π0b;


π0TGs π0p
{
  π0TS;
  π0av   v;  // ABI version being targeted by this program
  B      q;  // quoted statics
  V<π0b> p;  // bytecodes

  π0p() {}
  π0p(π0T(π0p) &&p_) : v(p_.v), q(std::move(p_.q)), p(std::move(p_.p)) {}
  π0p(π0av v_, Bc &q_, V<π0b> const &p_) : v(v_), q(q_), p(p_) {}
};


π0TGs π0h;        // GC heap (all generations)
π0TGs π0hs;       // GC heap space
π0TGs π0hv;       // GC heap view
π0TGs π0ms;       // GC mark set
π0TGs π0gs;       // GC splice map

typedef uN π0hg;  // heap generation
typedef uN π0ha;  // heap address within generation


π0TGs π0r         // π₀ generational heap reference
{
  π0TS;
  sletc rω   = Sc<uN>(-1);
  sletc gn   = Sc<uN>(1) << Gb;
  sletc ghms = τwordsize - Gb;
  sletc ghm  = gn - 1 << ghms;
  sletc gam  = ~ghm;
  static_assert((ghm | gam) == -1);
  static_assert((ghm & gam) ==  0);

  uN ga;
  π0r()                : ga(rω)             {}
  π0r(π0hg g, π0ha a_) : ga(g << ghms | a_) {}
  explicit π0r(uN ga_) : ga(ga_)            {}
  explicit π0r(i9 i)
  { let s = Sc<u9_scoped<u9_π, uN>>(i);
    A(s.t == u9_π::ref, "π₀r from non-ref " << i);
    ga = s.x; }

  π0hg g() const { return ga >> ghms; }
  uN   a() const { return ga  & gam;  }
  bool ω() const { return ga == rω; }

  explicit operator bool() const { return !ω(); }

  bool operator==(π0R x) const { return ga == x.ga; }
  bool operator<=(π0R x) const { return ga <= x.ga; }
  bool operator< (π0R x) const { return ga <  x.ga; }
  π0R  operator+ (iN  x) const { return π0R(g(), a() + x); }
  π0R  operator+ (uN  x) const { return *this + Sc<iN>(x); }
  iN   operator- (π0R x) const
  { A(g() == x.g(), "π0R- g mismatch: " << g() << " ≠ " << x.g());
    return a() - x.a(); }

  π0R &operator=(π0R const &r) { ga = r.ga; return *this; }
};


π0TGn struct u9t_<π0R> { sletc t = u9t::pi; };

π0TG inline auto o9(π0R r)
{
  return o9flag{o9(u9_scoped<u9_π, uN>{u9_π::ref, r.ga})};
}


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


#include "pi0-end.hh"


#endif
