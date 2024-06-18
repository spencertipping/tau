#ifndef σserver_Ωh_h
#define σserver_Ωh_h

#ifndef τarch
# define σserver_Ωh_debug_instantiations
#endif

#include "Omega-io.hh"
#include "../prof.hh"
#include "../begin.hh"

namespace σ
{


#define Tkl template<class K, class L>


Tkl struct Ωh final
{
  typedef K const Kc;
  typedef L const Lc;

  struct kl; typedef kl const klc;
  struct  kl final { K k; L l; τ::SO operator<=>(klc&) const = default; };
  sletc klb = sizeof(kl);


  Ωh(τ::Stc &path,
     bool    rw     = false,
     τ::f64  auto_f = 0.25,      // repack up to 25% of file
     τ::u32  mss    = 1048576);  // max stage size (elements)
  ~Ωh();

  void   add     (Kc&, Lc&);
  τ::u32 get     (Kc&, L*, τ::u32) const;
  void   commit  (bool fsync = false);
  void   repack  (τ::u64 max_bytes, bool fsync = false);
  τ::u64 unpacked() const;


protected:
  struct hd final
  {
    τ::ch   magic[4];
    τ::u32b rev;
    τ::u32b cap;
    τ::u32b n;
  };

  struct ss_
  {
    virtual ~ss_() = default;
    virtual explicit operator bool() const = 0;
    virtual kl       operator*    () const = 0;
    virtual ss_     &operator++   ()       = 0;
  };
  typedef τ::Sp<ss_> ss;

  struct ar final
  {
    τ::u64b o;
    τ::u64b s;

    τ::u64b n     ()                  const { return s / klb; }
    kl      at    (Ωfmc &m, τ::u64 i) const { return *τ::uap<klc>(m + (o + i * klb)); }
    ss      stream(Ωfmc &m)           const;
  };

  typedef hd const hdc;  sletc hdb = sizeof(hd);  static_assert(hdb == 16);
  typedef ar const arc;  sletc arb = sizeof(ar);  static_assert(arb == 16);

  sletc cap_ = 255;               // max #arrays capacity, constant
  sletc a0o_ = hdb + cap_ * arb;  // first array offset
  static_assert(a0o_ == 4096);


  τ::Stc         f_;
  Ωfd            fd_;
  bool           rw_;
  Ωfl            revl_;      // rev-lock, held for writer's lifetime
  τ::f64         auto_f_;    // auto-repack factor (see spec)
  Ωfm            map_;
  τ::u32         mss_;       // max stage size (#elements)
  τ::u32         rev_;       // last-read revision (readers)
  mutable τ::Smu as_mu_;
  τ::V<ar>       as_;        // active arrays, no ordering
  mutable τ::Smu stage_mu_;
  τ::MM<K, L>    stage_;     // staged insertions

  // NOTE: if stage_mu_ and ar_mu_ both need to be locked, always acquire
  // stage_mu_ first.
  //
  // NOTE: acquire file locks last because these span processes.


  τ::u32 search_in_(arc&, Kc&, L*, τ::u32) const;

  Ωfl    lock_arrays_ (bool rw) const { return {fd_, rw, hdb, arb * cap_}; }
  bool   read_header_ ();
  void   write_header_(bool fsync);
  void   write_arrays_(bool fsync);
  void   commit_      (bool fsync);                    // stage_mu_ is unique-locked
  void   repack_      (τ::u64 max_bytes, bool fsync);  // ar_mu_ is unique-locked
  τ::u64 insert_at_   (τ::u64 bytes) const;            // ar_mu_ is u/s-locked

  static ss merge_(ss, ss);
};


Tkl Ωh<K, L>::Ωh(τ::Stc &f, bool rw, τ::f64 auto_f, τ::u32 mss)
  : f_     (f),
    fd_    (Ωopen(f, rw)),
    rw_    (rw),
    revl_  (fd_, rw_),
    auto_f_(auto_f),
    map_   (fd_, false),
    mss_   (mss),
    rev_   (0)
{
  using namespace τ;
  if (rw_)
  {
    revl_.lock(4, 4);  // claim unique write access
    if (fd_->size() < hdb) write_header_(false);
  }
  else
  {
    A(fd_->size() >= hdb, "Ωh::Ωh(ro): no data in " << f);
    read_header_();
  }
}

Tkl Ωh<K, L>::~Ωh()
{
  if (rw_) commit(true);
}


Tkl typename Ωh<K, L>::ss Ωh<K, L>::ar::stream(Ωfmc &m) const
{
  struct ars final : public virtual ss_
  {
    ars(Ωfmc &m, τ::u64 o, τ::u64 s)
      : ss_(), m_(m), o_(o), s_(s), i_(0) {}

    explicit operator bool() const { return i_ * klb < s_; }
    kl       operator*    () const { return *τ::uap<klc>(m_ + (o_ + i_ * klb)); }
    ss_     &operator++   ()       { ++i_; return *this; }

  protected:
    Ωfmc  &m_;
    τ::u64 o_;
    τ::u64 s_;
    τ::u64 i_;  // current element
  };

  return std::make_shared<ars>(m, o, s);
}


Tkl typename Ωh<K, L>::ss Ωh<K, L>::merge_(ss a, ss b)
{
  struct ms final : public virtual ss_
  {
    ms(ss a, ss b) : a_(a), b_(b),
                     ae_(!*a_),
                     be_(!*b_),
                     av_(ae_ ? kl{0, {}} : **a_),
                     bv_(be_ ? kl{0, {}} : **b_) {}

    explicit operator bool() const { return !ae_ && !be_; }
    kl       operator*    () const { return ae_ ? bv_ : be_ ? av_ : std::min(av_, bv_); }
    ss_     &operator++   ()
      { let av0 = av_;
        let ae0 = ae_;
        if (!ae_ && (be_ || av_ <= bv_) && !(ae_ = !++*a_)) av_ = **a_;
        if (!be_ && (ae0 || bv_ <= av0) && !(be_ = !++*b_)) bv_ = **b_;
        return *this; }

  protected:
    ss   a_;
    ss   b_;
    bool ae_;
    bool be_;
    kl   av_;
    kl   bv_;
  };

  return std::make_shared<ms>(a, b);
}


Tkl void Ωh<K, L>::add(Kc &k, Lc &l)
{
  using namespace τ;
  Ul<Smu> sl(stage_mu_);
  stage_.insert({k, l});
  if (stage_.size() > mss_) commit_(false);
}


Tkl τ::u32 Ωh<K, L>::get(Kc &k, L *l, τ::u32 n) const
{
  using namespace τ;
  u32 r = 0;

  {
    Sl<Smu> sl(stage_mu_);
    let     e = stage_.equal_range(k);
    for (auto i = e.first; i != e.second; ++i)
      if (l && r < n) l[r++] = i->second;
  }

  {
    let     fl = lock_arrays_(false);
    Sl<Smu> al(as_mu_);
    for (u32 i = 0; r < n && i < as_.size(); ++i)
      r += search_in_(as_[i], k, l ? l + r : 0, n - r);
  }
  return r;
}


Tkl void Ωh<K, L>::commit(bool fsync)
{
  using namespace τ;
  Ul<Smu> l(stage_mu_);
  commit_(fsync);
}

Tkl void Ωh<K, L>::commit_(bool fsync)
{
  using namespace τ;
  Ul<Smu> sl(stage_mu_);  if (stage_.empty()) return;
  Ul<Smu> al(as_mu_);
  if (as_.size() + 1 >= cap_) repack_(fd_->size() * auto_f_, fsync);

  let as = stage_.size() * klb;
  let ao = insert_at_(as);
  V<kl> kls;  kls.reserve(stage_.size());
  for (let &[k, l] : stage_) kls.push_back({k, l});
  std::sort(kls.begin(), kls.end(), [](klc &a, klc &b) { return a.k < b.k; });

  for (u32 i = 0; i < kls.size(); ++i) memcpy(map_.at(ao + i * klb), &kls[i], klb);
  map_.sync(ao, as, fsync);
  as_.push_back({ao, as});
  repack_(as * 3, fsync);  // repack iff ≥50% of next-largest array
  write_arrays_(fsync);
}


Tkl τ::u64 Ωh<K, L>::insert_at_(τ::u64 bytes) const
{
  using namespace τ;
  if (as_.empty()) return hdb + cap_ * arb;

  V<u32> aoi(as_.size());  // indexes of as_ by ascending offset
  for (u32 i = 0; i < as_.size(); ++i) aoi.push_back(i);
  std::sort(aoi.begin(), aoi.end(), [&](u32 a, u32 b) { return as_[a].o < as_[b].o; });

  // Now look for any gap large enough to accept the new array. By default we
  // append to the end of the file, but it's possible that we'll have a gap
  // large enough for the new array.
  for (u32 i = 0; i + 1 < aoi.size(); ++i)
    if (as_[aoi[i + 1]].o - (as_[aoi[i]].o + as_[aoi[i]].s) >= bytes)
      return as_[aoi[i]].o + as_[aoi[i]].s;
  return as_.back().o + as_.back().s;
}


Tkl void Ωh<K, L>::repack(τ::u64 max_bytes, bool fsync)
{
  using namespace τ;
  Ul<Smu> al(as_mu_);
  repack_(max_bytes, fsync);
}

Tkl void Ωh<K, L>::repack_(τ::u64 max_bytes, bool fsync)
{
  using namespace τ;

  // We always repack the smallest arrays first, so find out how many fit into
  // max_bytes and how big they would be if we combined them. Then figure out
  // whether they fit into an existing gap, or whether we should append.
  V<u32> asi(as_.size());  // indexes of as_ by ascending size
  for (u32 i = 0; i < as_.size(); ++i) asi.push_back(i);
  std::sort(asi.begin(), asi.end(), [&](u32 a, u32 b) { return as_[a].s < as_[b].s; });

  u64 bytes = 0;
  u32 n     = 0;
  for (; n < asi.size(); ++n)
  {
    if (bytes + as_[asi[n]].s > max_bytes) break;
    bytes += as_[asi[n]].s;
  }
  if (n < 2) return;

  map_.update();  // make sure we can read everything
  V<ss> sas(n);   // streams to merge
  for (u32 i = 0; i < n; ++i) sas.push_back(as_[asi[i]].stream(map_));
  ss m = balanced_apply(mo(sas), merge_);

  let ao = insert_at_(bytes);
  u32 j  = 0;
  for (; *m; ++*m, ++j)
  {
    let kl = **m;
    memcpy(map_.at(ao + j * klb), &kl, klb);
  }
  map_.sync(ao, j * klb, fsync);

  V<ar> nas;  // new arrays
  for (u32 i = n; i < as_.size(); ++i) nas.push_back(as_[asi[i]]);
  nas.push_back({ao, j * klb});
  as_ = nas;
  write_arrays_(fsync);
}


Tkl bool Ωh<K, L>::read_header_()
{
  using namespace τ;
  let h = *uap<hdc>(map_ + 0);
  if (h.rev == rev_) return false;
  else
  {
    Ul<Smu> al{as_mu_};
    let fl = lock_arrays_(false);
    map_.update();
    as_.resize(h.n);
    for (u32 i = 0; i < as_.size(); ++i)
      as_[i] = *uap<arc>(map_ + (hdb + arb*i));
    return true;
  }
}


Tkl void Ωh<K, L>::write_header_(bool fsync)
{
  using namespace τ;
  A(rw_ && revl_.locked(), "Ωh::write_header_: not writable");
  hd h;
  memcpy(h.magic, "Ωh\0", 4);
  h.rev = rev_;
  h.cap = cap_;
  h.n   = as_.size();
  fd_->pwrite(&h, hdb, 0);
  if (fsync) fd_->fdatasync();
}


Tkl void Ωh<K, L>::write_arrays_(bool fsync)
{
  using namespace τ;
  let al = lock_arrays_(true);
  St ab; ab.resize(cap_ * arb);
  for (u32 i = 0; i < as_.size(); ++i) memcpy(&ab[arb*i], &as_[i], arb);
  fd_->pwrite(ab.data(), cap_ * arb, hdb);
  write_header_(false);
  if (fsync) fd_->fdatasync();
}


Tkl τ::u32 Ωh<K, L>::search_in_(arc &a, Kc &k, L *ls, τ::u32 n) const
{
  using namespace τ;
  u32 r  = 0;
  u64 u  = a.n();
  u64 l  = 0;
  u64 ku = Nl<u64>::max();
  u64 kl = Nl<u64>::min();
  let kn = u64(k);

  while (ku > kl && u - l > 1)
  {
    let m  = std::min<u64>(u - 1, l + (u - l) * f64(kn - kl) / (ku - kl));
    let am = a.at(map_, m);
    if      (am.k < kn) { ku = am.k; u = m; }
    else if (am.k > kn) { kl = am.k; l = m + 1; }
    else
    {
      // We're within the range of keys, but we don't know that we're at the
      // beginning. Let's find the beginning and proceed to the end.
      for (l = m; l > 0 && a.at(map_, l - 1).k == kn; --l);
      for (; r < n && l < a.n() && a.at(map_, l).k == kn; ++l)
        ls[r++] = a.at(map_, l).l;
      return r;
    }
  }

  return 0;
}


#ifdef σserver_Ωh_debug_instantiations
  namespace
  {
    struct os
    {
      τ::u64 o;
      τ::u32 s;
      τ::SO operator<=>(os const&) const = default;
    };
  }

  template struct Ωh<τ::u64, τ::u64>;
  template struct Ωh<τ::u64, os>;
#endif


#undef Tkl


}

#include "../end.hh"

#endif
