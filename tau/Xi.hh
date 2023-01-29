#ifndef τΞ_h
#define τΞ_h


#include "Lambda.hh"
#include "psi.hh"
#include "xiio.hh"

#include "begin.hh"

namespace τ
{


// Default size of ξs created with Ξ
letc Ξc0 = 8192;


// TODO: we need a way to collect groups of related ξs under the same
// name without creating collisions. For example, diagnostic connections
// for each of a series of pipeline elements. Ξ should be able to express
// some hierarchy.


struct Ξ
{
  Ξ(Λ &l)       : l_(l)  {}
  Ξ(Ξ const &x) : l_(x.l()) { xs_ = x.xs_; }


  bool has(Stc &k)   const {                                   return xs_.contains(k); }
  Ξ   &rm (Stc &k)         {                     xs_.erase(k); return *this; }
  Ξ   &mv (Stc &f, Stc &t) { xs_[t] = xs_.at(f); xs_.erase(f); return *this; }
  Ξ   &ln (Stc &f, Stc &t) { xs_[t] = xs_.at(f);               return *this; }

  // Predefine a pipe so it will be strongly linked to the source ψ, which
  // doesn't yet exist. This is useful when you're constructing a Ξ that
  // has parameter/placeholder ξs that are wired up after Γ instantiation.
  Ξ &def(Stc &k, uN c = Ξc0) { at(k, c); return *this; }

  ξio &operator[](Stc &k)
    { A(has(k), "Ξ[] !∃: " << k); return *xs_.at(k); }


  // Operate on a subset of ξs, selected by regex against the name.
  // TODO: replace/augment these with variants that use a function.
  Ξ sel(Re const&);            // construct a sub-Ξ from a regex
  Ξ sel(Re const&, Ξ const&);  // replace a regex with a Ξ


  ξo o(Stc &k, Sp<ψ> q, uN c = Ξc0) { return at(k, c)->o(q); }
  ξi i(Stc &k, Sp<ψ> q, uN c = Ξc0)
    { // If the input is requested before the output exists, then this
      // ξ moves data backwards; that means we must weaken its reference
      // to the producing ψ to prevent a reference cycle.
      let h = has(k);
      let r = at(k, c);
      return h ? r->i(q) : r->i(q).weaken(); }


  Ξ &operator=(Ξ const &x) { xs_ = x.xs_; return *this; }

  Λ &l() const { return l_; }

  M<St, Sp<ξio>>::iterator       begin()       { return xs_.begin(); }
  M<St, Sp<ξio>>::iterator       end()         { return xs_.end(); }
  M<St, Sp<ξio>>::const_iterator begin() const { return xs_.begin(); }
  M<St, Sp<ξio>>::const_iterator end()   const { return xs_.end(); }


protected:
  Λ              &l_;
  M<St, Sp<ξio>>  xs_;


  Sp<ξio> &at(Stc &k, uN c)
    { if (!xs_.contains(k)) xs_[k].reset(new ξio(l(), c));
      else
      { auto &x = xs_.at(k)->inner_ξ();
        x.resize(std::max(c, x.capacity())); }
      return xs_.at(k); }
};


O &operator<<(O&, Ξ const&);


}

#include "end.hh"


#endif
