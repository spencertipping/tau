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
  kviat_(cback const &l, Sp<kvmmat_> mm)
    : at_(ct_index{}, l), mm(mm) {}

  void α(ηic &k, ηic &v, ξo o) override { mm->α(k, v, o); }
  void ι(ηic &k, ηic &v, ξo o) override { o.r() << k.all() << term(k)->all().all(); }
  void κ(ηic &k, ηic &v, ξo o) override { mm->κ(k, v, o); }
  void τ(ηic &x, ξo o)         override { mm->τ(x, o); }
  void ρ(ηic &n, ηic &q, ξo o) override
    { o.r() << q.one().all() << query(q.next())->take(n.ci()).all(); }
  void ω(ηic &k, ηic &v, ξo o) override { mm->ω(k, v, o); }


  ηsstream term(ηic &k) { return mm->ss(k); }


  // Query syntax:
  //
  // ("*" a b ...) → intersect a b ...
  // ("+" a b ...) → union a b ...
  // ("-" a b)     → difference a b
  // ("t" x)       → lookup term x
  ηsstream query(ηic &q)
    { let y = q.η();
      let c = y.s();
      if (c == "t") return term(y[1]);
      if (c == "-")
        return query(y[1].one()) - query(y[2].one());

      V<ηsstream> xs;
      for (let &x : y.next()) xs.push_back(query(x.one()));
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
