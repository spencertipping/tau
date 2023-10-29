#ifndef σpre_sstream_h
#define σpre_sstream_h

#include "../tau.hh"
#include "../tau/begin.hh"

namespace σ::pre
{

using namespace τ;


// Balanced application, so we get log(n) depth
template<class F, class T>
static T ηss_bin_apply(V<T> &&xs, F const &f)
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


sletc ηsstream_fast = true;


// Index iterator base
template<bool fast>
struct ηsstream_
{
  sletc fast_ordering = fast;
  typedef Co<fast, SO, PO> ord;

  ord compare(ηic &a, ηic &b)
    { if constexpr (fast) return a.fast_compare(b);
      else                return a <=> b; }

  bool eq(ord c)
    { if constexpr (fast) return c == SO::equal;
      else                return c == PO::equivalent; }

  bool lt(ord c)
    { if constexpr (fast) return c == SO::less;
      else                return c == PO::less; }

  bool gt(ord c)
    { if constexpr (fast) return c == SO::greater;
      else                return c == PO::greater; }

  bool nc(ord c)
    { if constexpr (fast) return false;
      else                return c == PO::unordered; }

  void cmp_error(ηic &a, ηic &b)
    { if constexpr (fast) τunreachable();
      else                A(0, a << " <=> " << b << " is undefined"); }


  virtual ~ηsstream_() {}

  virtual operator   bool() = 0;  // do we have a current item?
  virtual ηi   operator* () = 0;
  virtual void operator++() = 0;
};


// Read entries from ηi
template<bool fast>
struct ηisstream_ final : public virtual ηsstream_<fast>
{
  ηisstream_(ηic &x) : ηsstream_<fast>(), x(x.begin()), e(x.end()) {}

  operator   bool() override { return x != e; }
  ηi   operator* () override { return *x; }
  void operator++() override { ++x; }

  ηi::it x;
  ηi::it e;
};


template<bool fast>
struct ηsstream_intersect_ final : public virtual ηsstream_<fast>
{
  typedef ηsstream_<fast> base_;
  typedef Sp<base_>       sstream_;

  ηsstream_intersect_(sstream_ a, sstream_ b)
    : ηsstream_<fast>(), a(a), b(b) { sync(); }

  operator   bool() override { return *a && *b; }
  ηi   operator* () override { return (**a).one(); }
  void operator++() override { ++*a; ++*b; sync(); }

  void sync()
    { while (*a && *b)
      { let c = base_::compare((**a).one(), (**b).one());
        if      (base_::lt(c)) ++*a;
        else if (base_::gt(c)) ++*b;
        else if (base_::eq(c)) break;
        else                   base_::cmp_error((**a).one(), (**b).one()); } }

  sstream_ a;
  sstream_ b;
};


template<bool fast>
struct ηsstream_union_ final : public virtual ηsstream_<fast>
{
  typedef ηsstream_<fast> base_;
  typedef Sp<base_>       sstream_;

  ηsstream_union_(sstream_ a, sstream_ b)
    : ηsstream_<fast>(), a(a), b(b) {}

  operator   bool() override { return *a || *b; }
  ηi   operator* () override
    { if (!*a) return (**b).one();
      if (!*b) return (**a).one();
      let a_ = (**a).one();
      let b_ = (**b).one();
      let c = base_::compare(a_, b_);
      return base_::lt(c) || base_::eq(c) ? a_ : b_; }

  void operator++() override
    { if      (!*a) ++*b;
      else if (!*b) ++*a;
      else
      { let a_ = (**a).one();
        let b_ = (**b).one();
        let c = base_::compare(a_, b_);
        if      (base_::lt(c)) ++*a;
        else if (base_::gt(c)) ++*b;
        else if (base_::eq(c)) ++*a, ++*b;
        else                   base_::cmp_error(a_, b_); } }

  sstream_ a;
  sstream_ b;
};


template<bool fast>
struct ηsstream_diff_ final : public virtual ηsstream_<fast>
{
  typedef ηsstream_<fast> base_;
  typedef Sp<base_>       sstream_;

  ηsstream_diff_(sstream_ a, sstream_ b)
    : ηsstream_<fast>(), a(a), b(b) {}

  operator   bool() override { return *a; }
  ηi   operator* () override { return (**a).one(); }
  void operator++() override
    { ++*a;
      while (*a && *b)
      { let c = base_::compare((**a).one(), (**b).one());
        if      (base_::lt(c)) break;
        else if (base_::gt(c)) ++*b;
        else if (base_::eq(c)) ++*a, ++*b;
        else                   base_::cmp_error((**a).one(), (**b).one()); } }

  sstream_ a;
  sstream_ b;
};


template struct ηsstream_<true>;
template struct ηsstream_<false>;


template<bool fast>
Sp<ηsstream_<fast>> operator|(Sp<ηsstream_<fast>> a, Sp<ηsstream_<fast>> b)
{ return Sp<ηsstream_<fast>>{new ηsstream_union_<fast>(a, b)}; }

template<bool fast>
Sp<ηsstream_<fast>> operator&(Sp<ηsstream_<fast>> a, Sp<ηsstream_<fast>> b)
{ return Sp<ηsstream_<fast>>{new ηsstream_intersect_<fast>(a, b)}; }

template<bool fast>
Sp<ηsstream_<fast>> operator-(Sp<ηsstream_<fast>> a, Sp<ηsstream_<fast>> b)
{ return Sp<ηsstream_<fast>>{new ηsstream_diff_<fast>(a, b)}; }


typedef Sp<ηsstream_<ηsstream_fast>> ηsstream;


ηsstream ηisstream         (ηic&);
ηsstream ηsstream_union    (Vc<ηsstream>&);
ηsstream ηsstream_intersect(Vc<ηsstream>&);


}

#include "../tau/end.hh"

#endif
