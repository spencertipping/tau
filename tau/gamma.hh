#ifndef τγ_h
#define τγ_h


#include "tau.hh"
#include "Xi.hh"

#include "begin.hh"

namespace τ
{


// Virtual base for γ compilers
struct γ_
{
  virtual ~γ_() {}

  virtual void operator()(Ξ &x) const = 0;
  virtual St         name()     const = 0;
};


// Public-facing API for γ_: all γs we create and use will be instances
// of this class
struct γ
{
  γ(Sp<γ_> g_) : g(g_) { check(); }
  γ(γ_    *g_) : g(g_) { check(); }

  template<class T>
  Sp<T> as () const { return std::dynamic_pointer_cast<T>(g); }
  γ_   &get() const { return *g; }

  γ &operator=(γ const &g_) { g = g_.g; check(); return *this; }

  St name() const { return g->name(); }

  Ξ  &operator()(Ξ &x)   const { (*g)(x); return x; }
  Ξ  &operator()(Ξ &&x)  const { return (*this)(x); }
  τe &operator()(τe &t)  const { (*this)(Ξ{t}); return t; }
  τe &operator()(τe &&t) const { return (*this)(t); }


protected:
  Sp<γ_> g;

  void check() { A(g, "∅γ"); }
};


// Left-linked list of γs, used for compositions, addressed virtually
// unless we're disassembling the list
struct Γ_ : public virtual γ_
{
  Γ_(γ x)           : h(x)       {}
  Γ_(γ x, Sp<Γ_> y) : h(x), t(y) {}

  γ      const h;
  Sp<Γ_> const t;

  void operator()(Ξ &x) const { h(x); if (t) (*t)(x); }
  St         name()     const { return h.name() + (t ? t->name() : ""); }
};


γ operator|(γ a, γ b);

O &operator<<(O&, γ const&);


}

#include "end.hh"


#endif
