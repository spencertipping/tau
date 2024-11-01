#ifndef σserver_Ωh_h
#define σserver_Ωh_h

#define σserver_Ωh_debug_instantiations 0

#include "Omega-io.hh"
#include "../prof.hh"
#include "../begin.hh"

namespace σ
{


Tt concept Ωh_k = requires (T x, T y)
{
  { x <=> y }           -> std::convertible_to<τ::SO>;
  { Sc<τ::u64>(x) }     -> std::convertible_to<τ::u64>;
  { std::hash<T>{}(x) } -> std::convertible_to<τ::u64>;
};

Tt concept Ωh_l = requires (T x, T y)
{
  { x <=> y }           -> std::convertible_to<τ::SO>;
  { std::hash<T>{}(x) } -> std::convertible_to<τ::u64>;
};


#define Tkl template<::σ::Ωh_k K, ::σ::Ωh_l L>


Tkl struct Ωh_kl final
{
  K k;
  L l;
  τ::SO operator<=>(Ωh_kl const&) const = default;
};

Tkl using Ωh_klc = Ωh_kl<K, L> const;


Tkl struct Ωh final
{
  typedef K const Kc;
  typedef L const Lc;

  typedef Ωh_kl<K, L> kl;
  typedef kl const klc;
  sletc klb = sizeof(kl);

  struct hd final
  {
    τ::ch   magic[4];
    τ::u32b rev;
    τ::u32b cap;
    τ::u32b n;
  };

  struct ss_
  {
    virtual                   ~ss_()       = default;
    virtual explicit operator bool() const = 0;
    virtual kl       operator*    () const = 0;
    virtual ss_     &operator++   ()       = 0;
  };
  typedef τ::Sp<ss_> ss;

  struct ar final
  {
    τ::u64b o;
    τ::u64b s;

    τ::SO operator<=>(ar const&) const = default;
    τ::u64b n     ()                  const { return s / klb; }
    kl      at    (Ωfmc &m, τ::u64 i) const { return *τ::uap<klc>(m + (o + i * klb)); }
    ss      stream(Ωfmc &m)           const;
  };

  typedef hd const hdc;  sletc hdb = sizeof(hd);  static_assert(hdb == 16);
  typedef ar const arc;  sletc arb = sizeof(ar);  static_assert(arb == 16);

  sletc cap_ = 255;               // max #arrays capacity, constant
  sletc a0o_ = hdb + cap_ * arb;  // first array offset
  static_assert(a0o_ == 4096);


  Ωh(τ::Stc &path,
     bool    rw     = false,
     τ::f64  auto_f = 0.25,      // repack up to 25% of file
     τ::u32  mss    = 1048576);  // max stage size (elements)
  ~Ωh();

  void add   (Kc&, Lc&);
  bool get   (Kc&, τ::Fc<bool(Lc&)>&);
  void commit(bool fsync = false);
  void repack(τ::u64 max_bytes, bool fsync = false);

  τ::u32 fragments () const { τ::Sl<τ::Smu> l{as_mu_}; return as_.size(); }
  τ::f64 efficiency() const { return τ::f64(asize()) / τ::f64(fd_->size()); }
  τ::u64 asize     () const
    { τ::Sl<τ::Smu> l{as_mu_};
      τ::u64 ts = 0; for (let &a : as_) ts += a.s;
      return ts; }

  τ::Sp<measurement> prof_rlock_arrays     () const { return prof_rlock_arrays_; }
  τ::Sp<measurement> prof_wlock_arrays     () const { return prof_wlock_arrays_; }
  τ::Sp<measurement> prof_add              () const { return prof_add_; }
  τ::Sp<measurement> prof_commit           () const { return prof_commit_; }
  τ::Sp<measurement> prof_commit_sort_stage() const { return prof_commit_sort_stage_; }
  τ::Sp<measurement> prof_repack           () const { return prof_repack_; }
  τ::Sp<measurement> prof_get              () const { return prof_get_; }
  τ::Sp<measurement> prof_search_step      () const { return prof_search_step_; }
  τ::Sp<measurement> prof_search_cut       () const { return prof_search_cut_; }
  τ::Sp<measurement> prof_search_read      () const { return prof_search_read_; }


protected:
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

  τ::Sp<measurement>
    prof_rlock_arrays_,
    prof_wlock_arrays_,
    prof_add_,
    prof_commit_,
    prof_commit_sort_stage_,
    prof_repack_,
    prof_get_,
    prof_search_step_,
    prof_search_cut_,
    prof_search_read_;

  // NOTE: if stage_mu_ and ar_mu_ both need to be locked, always acquire
  // stage_mu_ first.
  //
  // NOTE: acquire file locks last because these span processes.


  sletc not_found = -1ull;

  τ::u64 search_in_(arc&, Kc&)                    const;
  bool   search_in_(arc&, Kc&, τ::Fc<bool(Lc&)>&) const;

  Ωfl    lock_arrays_ (bool rw) const
    { let t = (rw ? prof_wlock_arrays_ : prof_rlock_arrays_)->start();
      return {fd_, rw, hdb, arb * cap_}; }

  bool   read_header_ ();
  void   write_header_(bool fsync);
  void   write_arrays_(bool fsync);
  void   ensure_write_(τ::u64 size);
  void   commit_      (bool fsync);                    // stage_mu_ is unique-locked
  void   repack_      (τ::u64 max_bytes, bool fsync);  // ar_mu_ is unique-locked
  τ::u64 insert_at_   (τ::u64 bytes) const;            // ar_mu_ is u/s-locked
  ss     krange_      (arc&, Kc&) const;

  static ss merge_(ss, ss);  // union
  static ss join_ (ss, ss);  // intersection
};

Tkl using Ωhc = Ωh<K, L> const;


}


namespace std
{

Tkl struct hash<σ::Ωh_kl<K, L>>
{
  size_t operator()(σ::Ωh_klc<K, L> &x) const
    { return std::hash<K>{}(x.k) * 33 ^ std::hash<L>{}(x.l); }
};

}


namespace σ
{


Tkl Ωh<K, L>::Ωh(τ::Stc &f, bool rw, τ::f64 auto_f, τ::u32 mss)
  : f_     (f),
    fd_    (Ωopen(f, rw)),
    rw_    (rw),
    revl_  (fd_, rw_),
    auto_f_(auto_f),
    map_   (fd_, rw_),
    mss_   (mss),
    rev_   (0),
    prof_rlock_arrays_     (measurement_for(τ::ηm{} << "Ωh" << f << "rlock_arrays")),
    prof_wlock_arrays_     (measurement_for(τ::ηm{} << "Ωh" << f << "wlock_arrays")),
    prof_add_              (measurement_for(τ::ηm{} << "Ωh" << f << "add")),
    prof_commit_           (measurement_for(τ::ηm{} << "Ωh" << f << "commit")),
    prof_commit_sort_stage_(measurement_for(τ::ηm{} << "Ωh" << f << "commit_sort_stage")),
    prof_repack_           (measurement_for(τ::ηm{} << "Ωh" << f << "repack")),
    prof_get_              (measurement_for(τ::ηm{} << "Ωh" << f << "get")),
    prof_search_step_      (measurement_for(τ::ηm{} << "Ωh" << f << "search_step")),
    prof_search_cut_       (measurement_for(τ::ηm{} << "Ωh" << f << "search_cut")),
    prof_search_read_      (measurement_for(τ::ηm{} << "Ωh" << f << "search_read"))
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
    ars(Ωfmc &m, τ::u64 o, τ::u64 s) : ss_(), m_(m), o_(o), s_(s), i_(0) {}
    explicit operator bool() const { return i_ * klb < s_; }
    kl       operator*    () const { return *τ::uap<klc>(m_ + (o_ + i_ * klb)); }
    ss_     &operator++   () { ++i_; return *this; }

  protected:
    Ωfmc   &m_;
    τ::u64c o_;
    τ::u64c s_;
    τ::u64  i_;  // current element
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

    explicit operator bool() const { return !ae_ || !be_; }
    kl       operator*    () const { return ae_ ? bv_ : be_ ? av_ : std::min(av_, bv_); }
    ss_     &operator++   ()
      { if      (ae_)       nb();
        else if (be_)       na();
        else if (bv_ < av_) nb();
        else if (av_ < bv_) na();
        else                na(), nb();
        return *this; }

  protected:
    ss   a_;
    ss   b_;
    bool ae_;
    bool be_;
    kl   av_;
    kl   bv_;

    void na() { if (!(ae_ = !++*a_)) av_ = **a_; }
    void nb() { if (!(be_ = !++*b_)) bv_ = **b_; }
  };

  return std::make_shared<ms>(a, b);
}


Tkl typename Ωh<K, L>::ss Ωh<K, L>::join_(ss a, ss b)
{
  struct js final : public virtual ss_
  {
    js(ss a, ss b) : a_(a), b_(b), n_(*a_ && *b_) { n(); }

    explicit operator bool() const { return  n_; }
    kl       operator*    () const { return  v_; }
    ss_     &operator++   ()  { n(); return *this; }

  protected:
    ss   a_;
    ss   b_;
    bool n_;
    kl   v_;

    bool n()
      { while (n_ && **a_ != **b_)
        { while ((n_ &= (bool) *a_) && **a_ < **b_) ++*a_;
          while ((n_ &= (bool) *b_) && **b_ < **a_) ++*b_; }
        if (n_) v_ = **a_;
        return n_; }
  };

  return std::make_shared<js>(a, b);
}


Tkl typename Ωh<K, L>::ss Ωh<K, L>::krange_(arc &a, Kc &k) const
{
  struct as final : public virtual ss_
  {
    as(arc &a, Ωfmc &m, τ::u64 p, K k) : a_(a), m_(m), p_(p), k_(k) {}

    explicit operator bool() const { return p_ < a_.n() && a_.at(m_, p_).k == k_; }
    kl       operator*    () const { return                a_.at(m_, p_); }
    ss_     &operator++   () { ++p_; return *this; }

  protected:
    ar     a_;
    Ωfmc  &m_;
    τ::u64 p_;
    K      k_;
  };

  return std::make_shared<as>(a, map_, search_in_(a, k), k);
}


Tkl void Ωh<K, L>::add(Kc &k, Lc &l)
{
  using namespace τ;
  let t = prof_add_->start();
  Ul<Smu> sl(stage_mu_);
  stage_.insert({k, l});
  if (stage_.size() > mss_) commit_(false);
}


Tkl bool Ωh<K, L>::get(Kc &k, τ::Fc<bool(Lc&)> &f)
{
  using namespace τ;

  let t = prof_get_->start();

  if (rw_)  // read-only doesn't use a stage
  {
    Sl<Smu> sl(stage_mu_);
    let     e = stage_.equal_range(k);
    for (auto i = e.first; i != e.second; ++i)
      if (!f(i->second)) return false;
  }

  {
    let fl = lock_arrays_(false);
    read_header_();
    Sl<Smu> al(as_mu_);
    for (u32 i = 0; i < as_.size(); ++i)
      if (!search_in_(as_[i], k, f)) return false;
  }

  return true;
}


Tkl void Ωh<K, L>::commit(bool fsync)
{
  using namespace τ;
  Ul<Smu> l(stage_mu_);
  commit_(fsync);
}

Tkl void Ωh<K, L>::commit_(bool fsync)
{
  // NOTE: stage_mu_ is held uniquely when this function is called
  using namespace τ;
  if (stage_.empty()) return;

  let t = prof_commit_->start();
  Ul<Smu> al(as_mu_);
  if (as_.size() + 1 >= cap_) repack_(fd_->size() * auto_f_, fsync);

  V<kl> kls;  kls.reserve(stage_.size());
  {
    S<kl> klss;
    for (let &[k, l] : stage_)
      if (klss.insert({k, l}).second)
        kls.push_back({k, l});
  }

  let as = kls.size() * klb;
  let ao = insert_at_(as);

  {
    // Important: always make sure to sort by both k and l because this will
    // enable us to deduplicate later when we merge arrays.
    let ts = prof_commit_sort_stage_->start();
    std::sort(kls.begin(), kls.end());
  }

  // Important: we're writing to a memory mapping, so make sure (1) the file is
  // already large enough to contain the new array, and (2) we've updated the
  // mapping as well.
  ensure_write_(ao + as);

  for (u32 i = 0; i < kls.size(); ++i) memcpy(map_.at(ao + i * klb), &kls[i], klb);
  map_.sync(ao, as, fsync);
  as_.push_back({ao, as});
  repack_(as * 3, fsync);  // repack iff ≥50% of next-largest array
  write_arrays_(fsync);

  // Very important: clear the fucking stage. If you don't do this, Ω will be
  // very slow. Don't ask me how I know this, aside from it being really
  // obvious.
  stage_.clear();
}


Tkl void Ωh<K, L>::ensure_write_(τ::u64 size)
{
  using namespace τ;
  if (fd_->size() < size)
    A(ftruncate(fd_->fd(), size) != -1,
      "Ωh::ensure_write_: ftruncate(" << size << ") failed");
  map_.update();
}


Tkl τ::u64 Ωh<K, L>::insert_at_(τ::u64 bytes) const
{
  using namespace τ;
  if (as_.empty()) return a0o_;

  V<u32> aoi(as_.size());  // indexes of as_ by ascending offset
  for (u32 i = 0; i < as_.size(); ++i) aoi[i] = i;
  std::sort(aoi.begin(), aoi.end(), [&](u32 a, u32 b) { return as_[a].o < as_[b].o; });

  // Does the range fit between end of header and the first array? If so, use
  // that.
  if (bytes <= as_[aoi.front()].o - a0o_) return a0o_;

  // Now look for any gap large enough to accept the new array. By default we
  // append to the end of the file, but it's possible that we'll have a gap
  // large enough for the new array.
  let n1 = aoi.size() - 1;
  for (u32 i = 0; i + 1 < aoi.size(); ++i)
    if (as_[aoi[i + 1]].o - (as_[aoi[i]].o + as_[aoi[i]].s) >= bytes)
      return as_[aoi[i]].o + as_[aoi[i]].s;
  return as_[aoi[n1]].o + as_[aoi[n1]].s;
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

  let t = prof_repack_->start();

  // We always repack the smallest arrays first, so find out how many fit into
  // max_bytes and how big they would be if we combined them. Then figure out
  // whether they fit into an existing gap, or whether we should append.
  V<u32> asi(as_.size());  // indexes of as_ by ascending size
  for (u32 i = 0; i < as_.size(); ++i) asi[i] = i;
  std::sort(asi.begin(), asi.end(),
            [&](u32 a, u32 b) { return as_[a].s < as_[b].s; });

  u64 bytes = 0;
  u32 n     = 0;
  for (; n < asi.size(); ++n)
  {
    if (bytes + as_[asi[n]].s > max_bytes) break;
    bytes += as_[asi[n]].s;
  }
  if (n < 2) return;

  // Important: we're writing to a memory mapping, so make sure (1) the file is
  // already large enough to contain the new array, and (2) we've updated the
  // mapping as well.
  let ao = insert_at_(bytes);
  u32 j  = 0;
  ensure_write_(ao + bytes);

  // Important: read streams must refer to the map after ensure_write_ has been
  // called; otherwise we may refer to unmapped addresses.
  V<ss> sas(n);   // streams to merge
  for (u32 i = 0; i < n; ++i) sas[i] = as_[asi[i]].stream(map_);
  ss m = balanced_apply(mo(sas), merge_);

  bool first = true;
  for (kl last = {}; *m; ++*m)
  {
    let kl = **m;
    if (first || kl != last) memcpy(map_.at(ao + j++ * klb), &kl, klb);
    last = kl;
    first = false;
  }
  map_.sync(ao, j * klb, fsync);

  V<ar> nas;  // new arrays
  for (u32 i = n; i < as_.size(); ++i) nas.push_back(as_[asi[i]]);
  nas.push_back({ao, j * klb});

  // NOTE: sorting just for debuggability
  std::sort(nas.begin(), nas.end(), [](arc &a, arc &b) { return a.o < b.o; });
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
  h.rev = ++rev_;
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
  u64 max_extent = hdb + cap_ * arb;
  for (u32 i = 0; i < as_.size(); ++i)
  {
    max_extent = std::max<u64>(max_extent, as_[i].o + as_[i].s);
    memcpy(&ab[arb*i], &as_[i], arb);
  }
  fd_->pwrite(ab.data(), cap_ * arb, hdb);
  A(ftruncate(fd_->fd(), max_extent) != -1,
    "Ωh::write_arrays_: ftruncate(" << max_extent << ") failed");
  write_header_(false);
  if (fsync) fd_->fdatasync();
}


Tkl τ::u64 Ωh<K, L>::search_in_(arc &a, Kc &k) const
{
  using namespace τ;
  u64 u  = a.n();
  u64 l  = 0;
  u64 p  = u;  // initial value is OOB
  u64 ku = Nl<u64>::max();
  u64 kl = Nl<u64>::min();
  let kn = Sc<u64>(k);
  int mi = ubits(u);  // max #iterations before switching to binary search

  for (auto tc = prof_search_cut_->start();
       ku > kl && u > l;)
  {
    auto ts = prof_search_step_->start();

    // Use an interpolation search until we get to log₂(n) iterations; by that
    // point we can reasonably infer that the values are not well-distributed,
    // so we fall back to binary search to avoid O(n) worst-case complexity.
    u64 m;
    if (mi-- > 0)
    {
      let f = f64(kn - kl) / (ku - kl);  // factor within space
      let e = l + (u - l) * f;           // expected element position
      m = std::min<u64>(u - 1, e);
    }
    else
      m = (u + l) / 2;

    let am = a.at(map_, m);
    if      (k < am.k) { ku = Sc<u64>(am.k); u = m;        }
    else if (k > am.k) { kl = Sc<u64>(am.k); l = m + 1;    }
    else               {                     p = m; break; }
  }

  if (p >= l && p < u)
  {
    let tr = prof_search_read_->start();

    // We're within the range of keys, but we don't know that we're at the
    // beginning. Let's find the beginning and proceed to the end.
    //
    // Note that we're comparing keys directly at this point, since we've
    // already gotten 64 bits of bisection out of the key space.
    for (; p > l && a.at(map_, p - 1).k >= k; --p);
    if (a.at(map_, p).k == k) return p;
  }

  return not_found;
}


Tkl bool Ωh<K, L>::search_in_(arc &a, Kc &k, τ::Fc<bool(Lc&)> &f) const
{
  using namespace τ;
  u64 p = search_in_(a, k);
  for (Ωh_kl<K, L> x; p < a.n() && (x = a.at(map_, p)).k == k; ++p)
    if (!f(x.l)) return false;
  return true;
}


#if σserver_Ωh_debug_instantiations
  // NOTE: this happens only in the editor; we don't want to force these
  // specific instantiations for real builds

  namespace
  {
    struct os_small
    {
      τ::u64b o;
      τ::u32b s;
      τ::SO operator<=>(os_small const&) const = default;
    };

    struct os_large
    {
      τ::u64b o;
      τ::u64b s;
      τ::SO operator<=>(os_large const&) const = default;
    };
  }

}

namespace std
{

Tn struct hash<σ::os_small>
{
  τ::u64 operator()(σ::os_small const &x) const
    { return std::hash<τ::u64b>{}(x.o) ^ std::hash<τ::u32b>{}(x.s); }
};

Tn struct hash<σ::os_large>
{
  τ::u64 operator()(σ::os_large const &x) const
    { return std::hash<τ::u64b>{}(x.o) ^ std::hash<τ::u64b>{}(x.s); }
};

}


namespace σ
{

  template struct Ωh<τ::u64b, τ::u64b>;
  template struct Ωh<τ::u64,  τ::u64>;
  template struct Ωh<τ::u32b, τ::u32b>;
  template struct Ωh<τ::u32,  τ::u32>;
  template struct Ωh<τ::u32b, τ::u64b>;
  template struct Ωh<τ::u32,  τ::u64>;
  template struct Ωh<τ::u64b, os_small>;
  template struct Ωh<τ::u64b, os_large>;
#endif


#undef Tkl


}

#include "../end.hh"

#endif
