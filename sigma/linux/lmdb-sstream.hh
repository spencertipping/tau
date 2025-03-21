#ifndef σlmdb_sstream_h
#define σlmdb_sstream_h

#include "lmdb.hh"

#include "../begin.hh"

namespace σ
{


sletc ηsstream_fast = true;


// Index iterator base
template<bool fast>
struct ηsstream_
{
  using SO  = τ::SO;
  using PO  = τ::PO;
  using ηi  = τ::ηi;
  using ηm  = τ::ηm;
  using ηic = τ::ηic;

  sletc fast_ordering = fast;
  typedef τ::Co<fast, SO, PO> ord;

  static ord compare(ηic &a, ηic &b)
    { if constexpr (fast) return a.fast_compare(b);
      else                return a <=> b; }

  static bool eq(ord c)
    { if constexpr (fast) return c == SO::equal;
      else                return c == PO::equivalent; }

  static bool lt(ord c)
    { if constexpr (fast) return c == SO::less;
      else                return c == PO::less; }

  static bool gt(ord c)
    { if constexpr (fast) return c == SO::greater;
      else                return c == PO::greater; }

  static bool nc(ord c)
    { if constexpr (fast) return false;
      else                return c == PO::unordered; }

  static void cmp_error(ηic &a, ηic &b)
    { if constexpr (fast) τunreachable();
      else                A(0, a << " <=> " << b << " is undefined"); }


  virtual ~ηsstream_() {}

  virtual operator   bool() = 0;  // do we have a current item?
  virtual ηi   operator* () = 0;
  virtual void operator++() = 0;

  ηm all()         { ηm r;              while          (*this) r << (**this).all(), ++*this;      return r; }
  ηm take(τ::uN n) { τ::uN i = 0; ηm r; while (i < n && *this) r << (**this).all(), ++*this, ++i; return r; }
};


template<bool fast>
struct ηsstream_cmp_
{
  using ηic = τ::ηic;
  typedef ηsstream_<fast> cmp;
  bool comp (ηic &a, ηic &b) const { return cmp::lt(cmp::compare(a, b)); }
  bool equiv(ηic &a, ηic &b) const { return cmp::eq(cmp::compare(a, b)); }
  bool operator()(ηic &a, ηic &b) const { return comp(a, b); }
};


// Empty set
template<bool fast>
struct ηesstream_ final : public virtual ηsstream_<fast>
{
  ηesstream_() {}
  operator    bool() override { return false; }
  τ::ηi operator* () override { τunreachable(); }
  void  operator++() override { τunreachable(); }
};


// Read entries from ηi
template<bool fast>
struct ηisstream_ final : public virtual ηsstream_<fast>
{
  using ηi  = τ::ηi;
  using ηic = τ::ηic;
  ηisstream_(lmdb::v<ηi> const &v)
    : ηsstream_<fast>(), v(v), x(v.x.begin()), e(v.x.end()) {}

  operator   bool() override { return x != e; }
  ηi   operator* () override { return (*x).one(); }
  void operator++() override { ++x; }

  lmdb::v<ηi> v;
  ηi::it x;
  ηi::it e;
};


// Read entries from a set
template<bool fast>
struct ηsosstream_ final : public virtual ηsstream_<fast>
{
  Txs using So = τ::So<Xs...>;
  using ηi = τ::ηi;
  using ηm = τ::ηm;

  ηsosstream_(So<ηm, ηsstream_cmp_<fast>> const &x)
    : ηsstream_<fast>(), x(x.begin()), e(x.end()) {}

  operator   bool() override { return x != e; }
  ηi   operator* () override { return *x; }
  void operator++() override { ++x; }

  typename So<ηm, ηsstream_cmp_<fast>>::const_iterator x;
  typename So<ηm, ηsstream_cmp_<fast>>::const_iterator e;
};


template<bool fast>
struct ηsstream_intersect_ final : public virtual ηsstream_<fast>
{
  Tt using Sp = τ::Sp<T>;
  using ηi = τ::ηi;

  typedef ηsstream_<fast> base_;
  typedef Sp<base_>       sstream_;

  ηsstream_intersect_(sstream_ a, sstream_ b)
    : ηsstream_<fast>(), a(a), b(b) { sync(); }

  operator   bool() override { return *a && *b; }
  ηi   operator* () override { return (**a).one(); }
  void operator++() override { ++*a; ++*b; sync(); }

  void sync();

  sstream_ a;
  sstream_ b;
};


template<bool fast>
struct ηsstream_union_ final : public virtual ηsstream_<fast>
{
  Tt using Sp = τ::Sp<T>;
  using ηi = τ::ηi;

  typedef ηsstream_<fast> base_;
  typedef Sp<base_>       sstream_;

  ηsstream_union_(sstream_ a, sstream_ b)
    : ηsstream_<fast>(), a(a), b(b) {}

  operator   bool() override { return *a || *b; }
  ηi   operator* () override;
  void operator++() override;

  sstream_ a;
  sstream_ b;
};


template<bool fast>
struct ηsstream_diff_ final : public virtual ηsstream_<fast>
{
  Tt using Sp = τ::Sp<T>;
  using ηi = τ::ηi;

  typedef ηsstream_<fast> base_;
  typedef Sp<base_>       sstream_;

  ηsstream_diff_(sstream_ a, sstream_ b)
    : ηsstream_<fast>(), a(a), b(b) {}

  operator   bool() override { return *a; }
  ηi   operator* () override { return (**a).one(); }
  void operator++() override;

  sstream_ a;
  sstream_ b;
};


template struct ηsstream_<true>;
template struct ηsstream_<false>;


template<bool fast>
τ::Sp<ηsstream_<fast>> operator|(τ::Sp<ηsstream_<fast>> a,
                                 τ::Sp<ηsstream_<fast>> b)
{
  if (!*a) return b;
  if (!*b) return a;
  return τ::Sp<ηsstream_<fast>>{new ηsstream_union_<fast>(a, b)};
}

template<bool fast>
τ::Sp<ηsstream_<fast>> operator&(τ::Sp<ηsstream_<fast>> a,
                                 τ::Sp<ηsstream_<fast>> b)
{
  if (!*a) return a;
  if (!*b) return b;
  return τ::Sp<ηsstream_<fast>>{new ηsstream_intersect_<fast>(a, b)};
}

template<bool fast>
τ::Sp<ηsstream_<fast>> operator-(τ::Sp<ηsstream_<fast>> a,
                                 τ::Sp<ηsstream_<fast>> b)
{
  if (!*a || !*b) return a;
  return τ::Sp<ηsstream_<fast>>{new ηsstream_diff_<fast>(a, b)};
}


typedef τ::Sp<ηsstream_<ηsstream_fast>> ηsstream;

typedef ηsstream_cmp_<ηsstream_fast> ηsstream_cmp;


ηsstream ηesstream         ();
ηsstream ηisstream         (lmdb::v<τ::ηi> const&);
ηsstream ηsosstream        (τ::So<τ::ηm, ηsstream_cmp> const&);
ηsstream ηsstream_union    (τ::Vc<ηsstream>&);
ηsstream ηsstream_intersect(τ::Vc<ηsstream>&);


}

#include "../end.hh"

#endif
