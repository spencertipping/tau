#ifndef tau_utf9_valsearch_h
#define tau_utf9_valsearch_h


#include <cstdint>
#include <vector>

#include "errors.hh"
#include "val.hh"


#include "../module/begin.hh"

namespace tau::utf9
{


static size_t constexpr const not_found_bit = 1ull << sizeof(size_t) * 8 - 1;


template <class KF>
uint64_t binsearch(std::vector<val> const &vs,
                   val              const &k,
                   size_t                  l = 0,
                   size_t                  u = -1ul)
{
  KF kf;
  u = std::min(u, vs.size());
  while (u > l + 1)
  {
    let m = u + l >> 1;
    switch (kf(vs[m]).compare(k))
    {
    case  1: u = m; break;
    case -1: l = m; break;
    case  0: return m;
    }
  }
  return l | (kf(vs[l]).compare(k) ? not_found_bit : 0);
}


template <class KF>
uint64_t interpsearch(std::vector<val> const &vs,
                      val              const &k,
                      size_t                  l = 0,
                      size_t                  u = -1ul)
{
  KF kf;
  u = std::min(vs.size(), u);
  uint64_t hu = kf(vs[u - 1]).h();
  uint64_t hl = kf(vs[0]).h();
  let hk = k.h().h;

  // NOTE: interp-search for long distances where we're likely to choose
  // better splits; then delegate to binsearch to divide the final interval.
  while (u > l + 16)
  {
    let m  = std::min(u - 1, std::max(l + 1, static_cast<size_t>(l + (u - l) * (hk - hl) / (hu - hl))));
    let hm = kf(vs[m]).h();
    switch (hm.compare(hk))
    {
    case  1: u = m; hu = hm; break;
    case -1: l = m; hl = hm; break;
    case  0: return m;
    }
  }

  while (u > l + 1)
  {
    let m  = u + l >> 1;
    let hm = kf(vs[m]).h();
    switch (hm.compare(hk))
    {
    case  1: u = m; hu = hm; break;
    case -1: l = m; hl = hm; break;
    case  0: return m;
    }
  }

  return l | (kf(vs[l]).compare(k) ? not_found_bit : 0);
}


#if TAU_VAL_SUPPORTS_ORD_IDX
template <class KF>
val s_to(val      const &b,
         val      const &k,
         val      const &hk,
         uint64_t const h)
{
  if (hk.exists() && hk > k) throw internal_error("s_to hk>k");
  KF kf;
  if (!b.has_ibuf()) { let i = binsearch<KF>(*b.vt, k); return i & not_found_bit ? none : (*b.vt)[i]; }
  for (uint64_t o = b.ibegin() + h; o < b.iend(); o += b.b->len(o))
  {
    let v = val(*b.b, o);
    switch (k.compare(kf(v)))
    {
    case -1: break;
    case  0: return v;
    case  1: return none;
    }
  }
  return none;
}

template <class KF> inline val val::to(val const &k, val const &hk, uint64_t const h) const { return s_to<KF>(*this, k, hk, h); }

template <class KF> bool val::is_to() const
{
  KF kf;
  if (!len()) return true;
  val last = none;
  for (let &v : *this)
  {
    let k = kf(v);
    if (last.exists() && k < last) return false;
    last = k;
  }
  return true;
}

template <class KF> val val::make_to() const
{
  KF kf;
  let v = new std::vector<val>;
  v->reserve(len());
  for (let &x : *this) v->push_back(x);
  std::sort(v->begin(), v->end(),
            [=](val const &a, val const &b) { return kf(a) < kf(b); });
  return val(v);
}
#endif


template <class KF>
val s_th(val      const &b,
         val      const &k,
         val      const &hk,
         uint64_t const h)
{
  if (hk.exists() && hk > k) throw internal_error("s_th hk>k");
  KF kf;
  if (!b.has_ibuf()) { let i = interpsearch<KF>(*b.vt, k); return i & not_found_bit ? none : (*b.vt)[i]; }
  let kh = k.h().h;
  for (uint64_t o = b.ibegin() + h; o < b.iend(); o += b.b->len(o))
  {
    let v  = val(*b.b, o);
    let vh = kf(v).h().h;
    if (kh > vh)  return none;
    if (kh == vh) return v;
  }
  return none;
}


inline val val::tp(uint64_t i, uint64_t const hi, uint64_t const h) const
{
  if (!has_ibuf()) return (*vt)[i];
  if (hi > i)      throw internal_error("hi>i");
  let      e = iend();
  uint64_t o = ibegin() + h;
  if (o >= e) throw decoding_error("tp0", *b, o);
  while (i-- > hi) if ((o += b->len(o)) >= e) throw decoding_error("tp", *b, o);
  return val(*b, o);
}

template <class KF> inline val val::th(val const &k, val const &hk, uint64_t const h) const { return s_th<KF>(*this, k, hk, h); }


template <class KF> bool val::is_th() const
{
  KF kf;
  if (!len()) return true;
  hash lh = 0;
  for (let &v : *this)
  {
    let h = kf(v).h();
    if (h < lh) return false;
    lh = h;
  }
  return true;
}


template <class KF> val val::make_th() const
{
  struct hi { uint64_t h; uint64_t i; };
  static_assert(sizeof(hi) == sizeof(val));

  KF kf;
  let vh = new std::vector<hi>;
  vh->reserve(len());
  if (has_ibuf()) for (let &x : *this) vh->push_back(hi{kf(x).h(), x.i});
  else
  { uint64_t j = 0;
    for (let &x : *this) vh->push_back(hi{kf(x).h(), j++}); }

  std::sort(vh->begin(), vh->end(),
            [](hi const &a, hi const &b) { return a.h < b.h; });

  let vs = reinterpret_cast<std::vector<val>*>(vh);
  if (has_ibuf()) for (uint64_t i = 0; i < vs->size(); ++i) (*vs)[i] = val(*b, (*vh)[i].i);
  else            for (uint64_t i = 0; i < vs->size(); ++i) (*vs)[i] = (*vt)[(*vh)[i].i];

  return val(vs);
}


}

#include "../module/end.hh"


#endif
