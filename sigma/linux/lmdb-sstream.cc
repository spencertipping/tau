#include "lmdb-sstream.hh"
#include "../begin.hh"

namespace σ
{

using namespace τ;


template<bool fast>
void ηsstream_intersect_<fast>::sync()
{
  while (*a && *b)
  {
    let c = base_::compare((**a).one(), (**b).one());
    if      (base_::lt(c)) ++*a;
    else if (base_::gt(c)) ++*b;
    else if (base_::eq(c)) break;
    else                   base_::cmp_error((**a).one(), (**b).one());
  }
}


template<bool fast>
ηi ηsstream_union_<fast>::operator*()
{
  if (!*a) return (**b).one();
  if (!*b) return (**a).one();
  let a_ = (**a).one();
  let b_ = (**b).one();
  let c = base_::compare(a_, b_);
  return base_::lt(c) || base_::eq(c) ? a_ : b_;
}


template<bool fast>
void ηsstream_union_<fast>::operator++()
{
  if      (!*a) ++*b;
  else if (!*b) ++*a;
  else
  {
    let a_ = (**a).one();
    let b_ = (**b).one();
    let c = base_::compare(a_, b_);
    if      (base_::lt(c)) ++*a;
    else if (base_::gt(c)) ++*b;
    else if (base_::eq(c)) ++*a, ++*b;
    else                   base_::cmp_error(a_, b_);
  }
}


template<bool fast>
void ηsstream_diff_<fast>::operator++()
{
  ++*a;
  while (*a && *b)
  {
    let c = base_::compare((**a).one(), (**b).one());
    if      (base_::lt(c)) break;
    else if (base_::gt(c)) ++*b;
    else if (base_::eq(c)) ++*a, ++*b;
    else                   base_::cmp_error((**a).one(), (**b).one());
  }
}


template struct ηsstream_intersect_<true>;
template struct ηsstream_intersect_<false>;

template struct ηsstream_union_<true>;
template struct ηsstream_union_<false>;

template struct ηsstream_diff_<true>;
template struct ηsstream_diff_<false>;


ηsstream ηesstream()
{
  static ηsstream esstream_;
  if (!esstream_) esstream_ = ηsstream(new ηesstream_<ηsstream_fast>());
  return esstream_;
}

ηsstream ηisstream(lmdb::v<ηi> const &x)
{
  return x.x.empty() ? ηesstream() : ηsstream(new ηisstream_<ηsstream_fast>(x));
}

ηsstream ηsosstream(So<ηm, ηsstream_cmp> const &x)
{
  return x.empty() ? ηesstream() : ηsstream(new ηsosstream_<ηsstream_fast>(x));
}

ηsstream ηsstream_union(Vc<ηsstream> &xs)
{
  return balanced_apply(V<ηsstream>{xs}, [](ηsstream &x, ηsstream &y) { return x | y; });
}

ηsstream ηsstream_intersect(Vc<ηsstream> &xs)
{
  return balanced_apply(V<ηsstream>{xs}, [](ηsstream &x, ηsstream &y) { return x & y; });
}


}

#include "../end.hh"
