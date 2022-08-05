#ifndef tau_utf9_valsearch_h
#define tau_utf9_valsearch_h


#include "../types.hh"

#include "errors.hh"
#include "val.hh"


#include "../module/begin.hh"

namespace tau::utf9
{


uN constexpr const not_found_bit = 1ull << sizeof(uN) * 8 - 1;


template <class KF>
uN binsearch(V<val> const &vs,
             val    const &k,
             uN            l = 0,
             uN            u = Nl<uN>::max())
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
uN interpsearch(V<val> const &vs,
                val    const &k,
                uN            l = 0,
                uN            u = Nl<uN>::max())
{
  KF kf;
  u = std::min(vs.size(), u);
  u64 hu = kf(vs[u - 1]).h();
  u64 hl = kf(vs[0]).h();
  let hk = k.h().h;

  // NOTE: interp-search for long distances where we're likely to choose
  // better splits; then delegate to binsearch to divide the final interval.
  while (u > l + 16)
  {
    let m  = std::min(u - 1, std::max(l + 1, Sc<uN>(l + (u - l) * (hk - hl) / (hu - hl))));
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


template <class KF>
val s_th(val const &b,
         val const &k,
         val const &hk,
         uN  const h)
{
  if (hk.exists() && hk > k) return throw_internal_error<val>("s_th hk>k");
  KF kf;
  if (!b.has_ibuf()) { let i = interpsearch<KF>(*b.vt, k); return i & not_found_bit ? u9n : (*b.vt)[i]; }
  let kh = k.h().h;
  for (uN o = b.ibegin() + h; o < b.iend(); o += b.b->len(o))
  {
    let v  = val(*b.b, o);
    let vh = kf(v).h().h;
    if (kh > vh)  return u9n;
    if (kh == vh) return v;
  }
  return u9n;
}


inline val val::tp(uN i, uN const hi, uN const h) const
{
  if (!has_ibuf()) return (*vt)[i];
  if (hi > i)      throw internal_error("hi>i");
  let e = iend();
  uN  o = ibegin() + h;
  if (o >= e) return throw_decoding_error<val>("tp0", *b, o);
  while (i-- > hi) if ((o += b->len(o)) >= e) return throw_decoding_error<val>("tp", *b, o);
  return val(*b, o);
}

template <class KF> inline val val::th(val const &k, val const &hk, uN const h) const
{ return s_th<KF>(*this, k, hk, h); }


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
  struct hi0 { u64 h; uN i; };
  struct hi  { u64 h; uN i; u8 pad[sizeof(val) - sizeof(hi0)]; };  // shenanigans
  static_assert(sizeof(hi) == sizeof(val));

  KF kf;
  let vh = new V<hi>;
  vh->reserve(len());
  if (has_ibuf()) for (let &x : *this) vh->push_back(hi{kf(x).h(), x.i});
  else
  { uN j = 0;
    for (let &x : *this) vh->push_back(hi{kf(x).h(), j++}); }

  std::sort(vh->begin(), vh->end(),
            [](hi const &a, hi const &b) { return a.h < b.h; });

  let vs = reinterpret_cast<V<val>*>(vh);
  if (has_ibuf()) for (uN i = 0; i < vs->size(); ++i) (*vs)[i] = val(*b, (*vh)[i].i);
  else            for (uN i = 0; i < vs->size(); ++i) (*vs)[i] = (*vt)[(*vh)[i].i];

  return val(vs);
}


}

#include "../module/end.hh"


#endif
