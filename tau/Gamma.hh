#ifndef τΓ_h
#define τΓ_h


#include "tau.hh"
#include "Xi.hh"

#include "begin.hh"

namespace τ
{


// Virtual base for Γ compilers
struct Γ_
{
  virtual ~Γ_() {}

  virtual Ξ operator()(Ξ const&) const = 0;
  virtual St    name()           const = 0;
};


// Public-facing API for Γ_: all Γs we create and use will be instances
// of this class
struct Γ
{
  Γ(Sp<Γ_> g_) : g(g_) { check(); }
  Γ(Γ_    *g_) : g(g_) { check(); }

  template<class T>
  Sp<T> as () const { return std::dynamic_pointer_cast<T>(g); }
  Γ_   &get() const { return *g; }

  Γ &operator=(Γ const &g_) { g = g_.g; check(); return *this; }

  St name() const { return g->name(); }

  Ξ operator()(Ξ const &x) const { (*g)(x); return x; }


protected:
  Sp<Γ_> g;

  void check() { A(g, "∅Γ"); }
};


// Left-linked list of Γs, used for compositions, addressed virtually
// unless we're disassembling the list
struct Γs_ : public virtual Γ_
{
  Γs_(Γ x)           : h(x)       {}
  Γs_(Γ x, Sp<Γ_> y) : h(x), t(y) {}

  Γ      const h;
  Sp<Γ_> const t;

  Ξ operator()(Ξ const &x) const { h(x); if (t) (*t)(x); return x; }
  St    name()             const { return h.name() + (t ? t->name() : ""); }
};


Γ operator|(Γ a, Γ b);


O &operator<<(O&, Γ const&);


}

#include "end.hh"


#endif
