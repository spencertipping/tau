#include "pre-container.hh"
#include "begin.hh"

namespace σ::pre
{


struct nfat_ : public virtual at_
{
  nfat_() : at_(ct_fifo{}, cb_native{}) {}
  void α(ηic &x, ξo)   override { xs.push_back(x.one()); }
  void ι(ηic &x, ξo o) override { o << xs.front(); xs.pop_front(); }
  void τ(ηic &x, ξo o) override { xs.clear(); o.r() << ηsig::τ; }
  D<ηm> xs;
};


struct nnat_ : public virtual at_
{
  nnat_() : at_(ct_prio_min{}, cb_native{}) {}
  void α(ηic &x, ξo)   override { xs.push(x.one()); }
  void ι(ηic &x, ξo o) override { o << xs.top(); xs.pop(); }
  void τ(ηic &x, ξo o) override
    { while (!xs.empty()) xs.pop();
      o.r() << ηsig::τ; }
  PQ<ηm, std::greater<ηm>> xs;  // NOTE: greater == min queue
};

struct nxat_ : public virtual at_
{
  nxat_() : at_(ct_prio_max{}, cb_native{}) {}
  void α(ηic &x, ξo)   override { xs.push(x.one()); }
  void ι(ηic &x, ξo o) override { o << xs.top(); xs.pop(); }
  void τ(ηic &x, ξo o) override
    { while (!xs.empty()) xs.pop();
      o.r() << ηsig::τ; }
  PQ<ηm, std::less<ηm>> xs;  // NOTE: less = max queue
};


struct nsat_ : public virtual at_
{
  nsat_() : at_(ct_set{}, cb_native{}) {}
  void α(ηic &x, ξo)   override { xs.insert(x.one()); }
  void ω(ηic &x, ξo)   override { xs.erase(x.one()); }
  void ι(ηic &x, ξo o) override { o.r() << xs.contains(x.one()); }
  void τ(ηic &x, ξo o) override { xs.clear(); o.r() << ηsig::τ; }
  S<ηm> xs;
};


struct nmat_ : public virtual at_
{
  nmat_() : at_(ct_map{}, cb_native{}) {}
  void α(ηic &x, ξo)   override { xs[x.one()] = x.next(); }
  void ω(ηic &x, ξo)   override { xs.erase(x.one()); }
  void ι(ηic &x, ξo o) override { if (xs.contains(x.one())) o << xs[x.one()]; else o.r() << ηsig::τ; }
  void τ(ηic &x, ξo o) override { xs.clear(); o.r() << ηsig::τ; }
  M<ηm, ηm> xs;
};


Sp<at_> at(ctype t, cback b)
{
  return std::visit(fn {
      [&](auto,        auto)      { A(0, "@TB unsupported: " << t << ", " << b); return Sp<at_>(nullptr); },
      [&](ct_fifo,     cb_native) { return Sp<at_>(new nfat_()); },
      [&](ct_prio_min, cb_native) { return Sp<at_>(new nnat_()); },
      [&](ct_prio_max, cb_native) { return Sp<at_>(new nxat_()); },
      [&](ct_set,      cb_native) { return Sp<at_>(new nsat_()); },
      [&](ct_map,      cb_native) { return Sp<at_>(new nmat_()); }}, t, b);
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
    TA(s, "ctype Va<> OOB (internal error)");
  }
}


O &operator<<(O &s, cback const &b)
{
  switch (b.index())
  {
  case 0: return s << "native";
  case 1: return s << "d" << std::get<1>(b).n;
  case 2: return s << "S\"" << std::get<2>(b).db << ":" << std::get<2>(b).t << "\"";
    TA(s, "cback Va<> OOB (internal error)");
  }
}


O &operator<<(O &s, at_ const &a)
{
  return s << "@" << a.t_ << a.b_;
}


}

#include "end.hh"
