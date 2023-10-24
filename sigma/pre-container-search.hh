#ifndef σpre_container_search_h
#define σpre_container_search_h

#include "pre-container.hh"
#include "pre-spatial.hh"
#include "../tau/begin.hh"

namespace σ::pre
{


// Index iterator base
struct iit_
{
  virtual ~iit_() {}
  virtual operator   bool() = 0;  // do we have a current item?
  virtual ηi   operator* () = 0;
  virtual void operator++() = 0;
};


struct ηiit final : public virtual iit_
{
  ηiit(ηic &x) : iit_(), x(x.begin()), e(x.end()) {}

  operator   bool() override { return x != e; }
  ηi   operator* () override { return *x; }
  void operator++() override { ++x; }

  ηi::it x;
  ηi::it e;
};


// Intersect index iterators
struct iiit final : public virtual iit_
{
  iiit(Sp<iit_> a, Sp<iit_> b) : iit_(), a(a), b(b) { sync(); }

  operator   bool() override { return *a && *b; }
  ηi   operator* () override { return (**a).one(); }
  void operator++() override { ++*a; ++*b; sync(); }

  void sync()
    { while (*a && *b)
      { let c = (**a).one() <=> (**b).one();
        if      (c == PO::less)       ++*a;
        else if (c == PO::greater)    ++*b;
        else if (c == PO::equivalent) break;
        else A(0, (**a).one() << " <=> " << (**b).one() << " is undefined"); } }


  Sp<iit_> a;
  Sp<iit_> b;
};


// Union index iterators
struct uiit final : public virtual iit_
{
  uiit(Sp<iit_> a, Sp<iit_> b) : iit_(), a(a), b(b) {}

  operator   bool() override { return *a || *b; }
  ηi   operator* () override
    { if (!*a) return (**b).one();
      if (!*b) return (**a).one();
      let a_ = (**a).one();
      let b_ = (**b).one();
      return a_ <= b_ ? a_ : b_; }

  void operator++() override
    { if      (!*a) ++*b;
      else if (!*b) ++*a;
      else
      { let a_ = (**a).one();
        let b_ = (**b).one();
        let c = a_ <=> b_;
        if      (c == PO::less)       ++*a;
        else if (c == PO::greater)    ++*b;
        else if (c == PO::equivalent) ++*a, ++*b;
        else A(0, a_ << " <=> " << b_ << " is undefined"); } }

  Sp<iit_> a;
  Sp<iit_> b;
};


// Difference index iterators
struct diit final : public virtual iit_
{
  diit(Sp<iit_> a, Sp<iit_> b) : iit_(), a(a), b(b) {}

  operator   bool() override { return *a; }
  ηi   operator* () override { return (**a).one(); }
  void operator++() override
    { ++*a;
      while (*a && *b)
      { let c = (**a).one() <=> (**b).one();
        if      (c == PO::less)       break;
        else if (c == PO::greater)    ++*b;
        else if (c == PO::equivalent) ++*a, ++*b;
        else A(0, (**a).one() << " <=> " << (**b).one() << " is undefined"); } }

  Sp<iit_> a;
  Sp<iit_> b;
};


// Balanced application, so we get log(n) depth
template<class F, class T>
static T bin_apply(F const &f, V<T> &xs)
{
  while (xs.size() > 1)
  {
    V<T> ys;
    ys.reserve(xs.size() / 2 + 1);
    for (uN i = 0; i < xs.size(); i += 2)
      ys.push_back(f(xs[i], xs[i + 1]));
    if (xs.size() & 1) ys.push_back(xs.back());
    xs = ys;
  }
  return xs.back();
}


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
  Sp<iit_> query_(ηic&);


  Sp<kv_>      db;
  M<ηm, S<ηm>> xs;  // staged insertions
  uN           s;   // size of staged insertions
};


}

#include "../tau/end.hh"

#endif
