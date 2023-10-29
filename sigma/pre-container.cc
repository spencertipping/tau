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
      [&](ct_index,    cb_native) { return iat(cb_native{}, kv_native()); },

      [&](ct_set,               cb_lmdb const &l) { return lmdb_set(l); },
      [&](ct_map,               cb_lmdb const &l) { return lmdb_map(l); },
      [&](ct_multimap const &m, cb_lmdb const &l) { return lmdb_multimap(m, l); },
      [&](ct_index,             cb_lmdb const &l) { return iat(l, kv_lmdb(l.f, l.db)); }
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
