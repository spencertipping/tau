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
  virtual St      name()         const = 0;
};


// Public-facing API for Γ_: all Γs we create and use will be instances
// of this class
struct Γ final
{
  Γ(Sp<Γ_> g_) : g(g_) {}
  Γ(Γ_    *g_) : g(g_) {}

  St name() const { A(g, "Γ::name empty"); return g->name(); }
  Γ_ &get() const { A(g, "Γ::get empty");  return *g; }

  operator bool() const { return Sc<bool>(g); }

  template<class T = Γ_> Sp<T> as() const { return dpc<T>(g); }
  template<class T>      bool  is() const { return dynamic_cast<T*>(g.get()); }

  Γ &operator= (Γ const &g_)      { g = g_.g;          return *this; }
  Ξ  operator()(Ξ const &x) const { A(g, "Γ() empty"); return (*g)(x); }


protected:
  Sp<Γ_> g;
};


// Trivial C++ lambda implementation of Γ_
struct Γf_ : public virtual Γ_
{
  Γf_(St n_, F<Ξ(Ξ const&)> &&f_) : n(n_), f(std::move(f_)) {}

  Ξ operator()(Ξ const &x) const { return f(x); }
  St      name()           const { return n; }

protected:
  St             n;
  F<Ξ(Ξ const&)> f;
};


// Left-linked list of Γs, used for compositions, addressed virtually
// unless we're disassembling the list
struct Γs_ : public virtual Γ_
{
  Γs_(Γ x)            : h(x)       {}
  Γs_(Γ x, Sp<Γs_> y) : h(x), t(y) {}
  Γs_(Γ x, Γs_    *y) : h(x), t(y) {}

  Γ       const h;
  Sp<Γs_> const t;

  Ξ operator()(Ξ const &x) const { let y = h(x); return t ? (*t)(y) : y; }
  St    name()             const { return h.name() + (t ? t->name() : ""); }
};


// Associatively compose two Γs
Γ operator|(Γ a, Γ b);


O &operator<<(O&, Γ const&);


}

#include "end.hh"


#endif
