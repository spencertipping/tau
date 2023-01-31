#ifndef τΞ_h
#define τΞ_h


#include "Lambda.hh"
#include "tau.hh"
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

// Q: does it make sense for Ξ : ξio? Almost certainly not, but it would
// be one way to get hierarchy as well as a default ξ. Sort of like a
// default "unit" dimension for a vector space.

// Q: do we want to encourage many-ξs, or do we want multiplexed pairs?
// It seems like multiplexing should be preferred in many cases -- although
// we still need enough organizational machinery to route stuff to the
// multiplexer.
//
// Whether or not we ultimately use a multiplexer is immaterial; we should
// be able to hang onto the intermediate ξs easily and not be forced to
// delegate too early. That means we need some structured organization.


struct Ξ
{
  Ξ(τe &t)      : t_(t)  {}
  Ξ(Ξ const &x) : t_(x.t()) { xs_ = x.xs_; }

  Ξ &operator=(Ξ const &x) { xs_ = x.xs_; return *this; }

  Λ  &l() const { return t_.l(); }
  τe &t() const { return t_; }


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
  Ξ sel(Re const&) const;            // construct a sub-Ξ from a regex
  Ξ sel(Re const&, Ξ const&) const;  // replace a regex with a Ξ


  ξo o(Stc &k, Sp<ψ> q, uN c = Ξc0) { return at(k, c)->o(q); }
  ξi i(Stc &k, Sp<ψ> q, uN c = Ξc0)
    { // If the input is requested before the output exists, then this
      // ξ moves data backwards; that means we must weaken its reference
      // to the producing ψ to prevent a reference cycle.
      let h = has(k);
      let r = at(k, c);
      return h ? r->i(q) : r->i(q).weaken(); }


  M<St, Sp<ξio>>::iterator       begin()       { return xs_.begin(); }
  M<St, Sp<ξio>>::iterator       end()         { return xs_.end(); }
  M<St, Sp<ξio>>::const_iterator begin() const { return xs_.begin(); }
  M<St, Sp<ξio>>::const_iterator end()   const { return xs_.end(); }


protected:
  τe             &t_;
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
