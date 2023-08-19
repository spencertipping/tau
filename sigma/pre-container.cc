#include "pre-container.hh"
#include "pre-spatial.hh"
#include "begin.hh"

namespace σ::pre
{


struct nfat_ : public virtual at_
{
  nfat_() : at_(ct_fifo{}, cb_native{}) {}
  void α(ηic &x, ηic&, ξo)   override { xs.push_back(x.one()); }
  void ι(ηic &x, ηic&, ξo o) override { o << xs.front(); xs.pop_front(); }
  void τ(ηic &x,       ξo o) override { xs.clear(); o.r() << ηsig::τ; }
  D<ηm> xs;
};


struct nnat_ : public virtual at_
{
  nnat_() : at_(ct_prio_min{}, cb_native{}) {}
  void α(ηic &x, ηic&, ξo)   override { xs.push(x.one()); }
  void ι(ηic &x, ηic&, ξo o) override { o << xs.top(); xs.pop(); }
  void τ(ηic &x,       ξo o) override
    { while (!xs.empty()) xs.pop();
      o.r() << ηsig::τ; }
  PQ<ηm, std::greater<ηm>> xs;  // NOTE: greater == min queue
};

struct nxat_ : public virtual at_
{
  nxat_() : at_(ct_prio_max{}, cb_native{}) {}
  void α(ηic &x, ηic&, ξo)   override { xs.push(x.one()); }
  void ι(ηic &x, ηic&, ξo o) override { o << xs.top(); xs.pop(); }
  void τ(ηic &x,       ξo o) override
    { while (!xs.empty()) xs.pop();
      o.r() << ηsig::τ; }
  PQ<ηm, std::less<ηm>> xs;  // NOTE: less = max queue
};


struct nsat_ : public virtual at_
{
  nsat_() : at_(ct_set{}, cb_native{}) {}
  void α(ηic &x, ηic&, ξo)   override { xs.insert(x.one()); }
  void ω(ηic &x, ηic&, ξo)   override { xs.erase(x.one()); }
  void ι(ηic &x, ηic&, ξo o) override { o.r(x.lsize() + 2) << x.all() << xs.contains(x.one()); }
  void τ(ηic &x,       ξo o) override { xs.clear(); o.r() << ηsig::τ; }
  S<ηm> xs;
};

struct nuat_ : public virtual at_
{
  nuat_() : at_(ct_uniq{}, cb_native{}) {}
  void α(ηic &x, ηic&, ξo o) override { if (xs.insert(x.one()).second) o << x.one(); }
  void τ(ηic &x,       ξo o) override { xs.clear(); o.r() << ηsig::τ; }
  S<ηm> xs;
};


struct nmat_ : public virtual at_
{
  nmat_() : at_(ct_map{}, cb_native{}) {}
  void α(ηic &k, ηic &v, ξo) override { xs[k] = v; }
  void ω(ηic &k, ηic&, ξo)   override { xs.erase(k); }
  void ι(ηic &k, ηic&, ξo o) override
    { if (xs.contains(k.one())) o.r(k.lsize() + 64) << k.all() << xs[k].all();
      else                      o.r(k.lsize() + 2)  << k.all() << ηsig::ω; }
  void τ(ηic &x,       ξo o) override { xs.clear(); o.r() << ηsig::τ; }
  M<ηm, ηm> xs;
};


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

  void τ(ηic &x, ξo o)       override { commit(); db->sync(); o.r() << ηsig::τ; }

  void staged(uN n) { if ((s += n) > 1 << 20) commit(), s = 0; }
  void commit();

  Sp<kv_>      db;
  M<ηm, S<ηm>> xs;  // staged insertions
  uN           s;   // size of staged insertions
};


void kviat_::commit()
{
  if (!s) return;
  ηm x;
  for (let &[k, vs] : xs)
  {
    x.clear();
    V<ηi> av(vs.begin(), vs.end());
    let   bv = db->get(k);
    std::sort(av.begin(), av.end());

    uN t = bv.lsize();
    for (let &v : av) t += v.osize();
    x.reserve(t);

    auto ai = av.begin();
    auto bi = bv.begin();
    let  ae = av.end();
    let  be = bv.end();

    while (ai != ae && bi != be)
    {
      let a = (*ai).one();
      let b = (*bi).one();
      let c = a <=> b;
      if      (c == PO::less)       x << a.all(), ++ai;
      else if (c == PO::greater)    x << b.all(), ++bi;
      else if (c == PO::equivalent) x << a.all(), ++ai, ++bi;
      else A(0, a << " <=> " << b << " is undefined");
    }
    while (ai != ae) x << (*ai++).one().all();
    while (bi != be) x << (*bi).one().all(), ++bi;

    db->set(k, x);
  }

  xs.clear();
  s = 0;
}


Sp<at_> at(ctype t, cback b)
{
  return std::visit(fn {
      [&](let&, let&) { A(0, "@TB unsupported: " << t << ", " << b); return Sp<at_>(nullptr); },

      [&](ct_fifo,     cb_native) { return Sp<at_>(new nfat_()); },
      [&](ct_prio_min, cb_native) { return Sp<at_>(new nnat_()); },
      [&](ct_prio_max, cb_native) { return Sp<at_>(new nxat_()); },
      [&](ct_set,      cb_native) { return Sp<at_>(new nsat_()); },
      [&](ct_uniq,     cb_native) { return Sp<at_>(new nuat_()); },
      [&](ct_map,      cb_native) { return Sp<at_>(new nmat_()); },
      [&](ct_index,    cb_native) { return Sp<at_>(new kviat_(cb_native{}, kv_native())); },

      [&](ct_set,   cb_lmdb const &l) { return lmdb_set(l); },
      [&](ct_map,   cb_lmdb const &l) { return lmdb_map(l); },
      [&](ct_index, cb_lmdb const &l) { return Sp<at_>(new kviat_(l, kv_lmdb(l.f, l.db))); }
    }, t, b);
}


Sp<at_ls_> at_ls(cback b)
{
  return std::visit(fn {
      [&](let&) { A(0, "@|B unsupported: " << b); return Sp<at_ls_>(nullptr); },
      [&](cb_lmdb const &l) { return lmdb_ls(l); }}, b);
}


O &operator<<(O &s, ctype const &t)
{
  switch (t.index())
  {
  case 0: return s << "=";
  case 1: return s << "<";
  case 2: return s << ">";
  case 3: return s << "B" << std::get<3>(t).n << "," << std::get<3>(t).fp;
  case 4: return s << "u";
  case 5: return s << "?";
  case 6: return s << ":";
  case 7: return s << ";";
  case 8: return s << "S";
  case 9: return s << "?";
    TA(s, "ctype Va<> OOB (internal error)");
  }
}


O &operator<<(O &s, cback const &b)
{
  switch (b.index())
  {
  case 0: return s << "native";
  case 1: return s << "d" << std::get<1>(b).n;
  case 2: return s << "S\"" << std::get<2>(b).db << ":" << std::get<2>(b).t  << "\"";
  case 3: return s << "L\"" << std::get<3>(b).f  << ":" << std::get<3>(b).db << "\"";
    TA(s, "cback Va<> OOB (internal error)");
  }
}


O &operator<<(O &s, at_ const &a)
{
  return s << "@" << a.t_ << a.b_;
}


}

#include "end.hh"
