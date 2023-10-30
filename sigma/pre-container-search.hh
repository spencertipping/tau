#ifndef σpre_container_search_h
#define σpre_container_search_h

#include "pre-container.hh"
#include "pre-container-kv.hh"
#include "pre-sstream.hh"
#include "../tau/begin.hh"

namespace σ::pre
{


struct kviat_ : public virtual at_
{
  kviat_(cback const &l, Sp<kvmmat_> mm) : at_(ct_index{}, l), mm(mm) {}

  void α(ηic &k, ηic &v, ξo o) override { mm->α(k, v, o); }
  void ι(ηic &k, ηic &v, ξo o) override { o.r() << k.all() << term(k)->all(); }
  void κ(ηic &k, ηic &v, ξo o) override { mm->κ(k, v, o); }
  void τ(ηic &x, ξo o)         override { mm->τ(x, o); }
  void ρ(ηic &n, ηic &q, ξo o) override
    { o.r() << q.one().all() << query(n.ci(), q.next()).all(); }


  ηsstream term(ηic &k) { return mm->ss(k); }


  // Query syntax:
  //
  // ("*" a b ...) → intersect a b ...
  // ("+" a b ...) → union a b ...
  // ("-" a b)     → difference a b
  // ("t" x)       → lookup term x
  ηm       query (uN n, ηic &q) { return query_(q)->all(); }
  ηsstream query_(ηic &q)
  { let c = q.η().s();
    if (c == "t") return term(q.η().next());
    if (c == "-")
      return query_(q.η().next()) - query_(q.η().next().next());

    V<ηsstream> xs;
    for (let &x : q.η().next()) xs.push_back(query_(x));
    if      (c == "+") return ηsstream_union(xs);
    else if (c == "*") return ηsstream_intersect(xs);
    else
    { A(0, "unknown query operator: " << c << " in " << q);
      τunreachable(); } }


  Sp<kvmmat_> mm;
};


}

#include "../tau/end.hh"

#endif
