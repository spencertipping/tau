#ifndef σpre_container_search_h
#define σpre_container_search_h

#include "pre-container.hh"
#include "pre-spatial.hh"
#include "pre-sstream.hh"
#include "../tau/begin.hh"

namespace σ::pre
{


// TODO: add key overflow
// TODO: add multi-layer staging (can just be keyspace, not separate tables)
struct kviat_ : public virtual at_
{
  kviat_(cback const &l, Sp<kv_> db) : at_(ct_map{}, l), db(db), s(0) {}
  ~kviat_() { commit(); }

  void α(ηic &k, ηic &v, ξo) override { if (xs[k].insert(v).second) staged(v.lsize()); }
  void ι(ηic &k, ηic&, ξo o) override
    { commit();
      let r = db->get(k);
      if (!r.empty()) o.r(k.lsize() + r.size() + 8) << k.all() << r.all();
      else            o.r(k.lsize() + 2)            << k.all() << ηsig::ω; }

  void τ(ηic &x, ξo o)         override { commit(); db->sync(); o.r() << ηsig::τ; }
  void ρ(ηic &n, ηic &q, ξo o) override { commit(); o.r() << q.one().all() << query(n.ci(), q.next()).all(); }


  ηi   at(ηic &k)   { commit(); return db->get(k); }
  void staged(uN n) { if ((s += n) > 1 << 20) commit(); }
  void commit();

  ηm       query (uN n, ηic&);
  ηsstream query_(ηic&);


  Sp<kv_>      db;
  M<ηm, S<ηm>> xs;  // staged insertions
  uN           s;   // size of staged insertions
};


}

#include "../tau/end.hh"

#endif
