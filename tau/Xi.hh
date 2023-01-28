#ifndef τΞ_h
#define τΞ_h


#include "Lambda.hh"
#include "psi.hh"
#include "xiio.hh"

#include "begin.hh"

namespace τ
{


// Default size of ξs created with Ξ
sletc Ξc0 = 8192;


struct Ξ
{
  Ξ(Λ &l_)      : l(l_)  {}
  Ξ(Ξ const &x) : l(x.l) { xs = x.xs; }

  Ξ cp() const { return *this; }

  bool has (Stc &k)   const {                                return xs.contains(k); }
  Ξ   &rm  (Stc &k)         {                   xs.erase(k); return *this; }
  Ξ   &mv  (Stc &f, Stc &t) { xs[t] = xs.at(f); xs.erase(f); return *this; }

  // Predefine a pipe so it will be strongly linked to the source ψ, which
  // doesn't yet exist. This is useful when you're constructing a Ξ that
  // has parameter/placeholder ξs that are wired up after Γ instantiation.
  Ξ &def (Stc &k, uN c = Ξc0) { at(k, c); return *this; }


  ξo o(Stc &k, Sp<ψ> q, uN c = Ξc0) { return at(k, c)->o(q); }
  ξi i(Stc &k, Sp<ψ> q, uN c = Ξc0)
    { // If the input is requested before the output exists, then this
      // ξ moves data backwards; that means we must weaken its reference
      // to the producing ψ to prevent a reference cycle.
      let h = has(k);
      let r = at(k, c);
      return h ? r->i(q) : r->i(q).weaken(); }


protected:
  Λ              &l;
  M<St, Sp<ξio>>  xs;

  friend O &operator<<(O&, Ξ const&);

  Sp<ξio> &at(Stc &k, uN c)
    { if (!xs.contains(k)) xs[k].reset(new ξio(l, c, k));
      else
      { let c0 = xs.at(k)->capacity();
        A(c <= c0, "ξ capacity mismatch: " << c << " > " << c0); }
      return xs.at(k); }
};


O &operator<<(O &s, Ξ const &x)
{
  s << "Ξ[";
  bool first = true;
  for (let &[k, io] : x.xs)
  { if (first) first = false;
    else       s << " ";
    s << k << "|" << (io->can_i() ? "i" : "")
                  << (io->can_o() ? "o" : ""); }
  return s << "]";
}


}

#include "end.hh"


#endif
