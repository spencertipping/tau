#ifndef σserver_Ωh_h
#define σserver_Ωh_h

#include "Omega-io.hh"
#include "../begin.hh"

namespace σ
{


#define Tkl template<class K, class L>


Tkl struct Ωh final
{
  typedef K const Kc;
  typedef L const Lc;

  struct kl final { K k; L l; };
  sletc  klb = sizeof(kl);


  Ωh(τ::Stc &path,
     bool    rw     = false,
     τ::f64  auto_f = 1.5,
     τ::u32  mss    = 1048576,
     τ::u32  cap    = 255);
  ~Ωh();

  void   add     (Kc&, Lc&);
  τ::u32 get     (Kc&, L*, τ::u32) const;
  void   commit  ();        // commit the stage to a new array
  void   repack  (τ::u64);  // repack up to this many _bytes_
  τ::u64 unpacked() const;  // how many _bytes_ of k/v entries are unpacked


protected:
  struct header final
  {
    τ::ch   magic[4];
    τ::u32b rev;
    τ::u32b cap;
    τ::u32b n;
  };

  struct array final
  {
    τ::u64b o;
    τ::u64b s;
  };

  static_assert(sizeof(header) == 16);
  static_assert(sizeof(array)  == 16);


  τ::Stc         f_;
  Ωfd            fd_;
  bool           rw_;
  Ωfl            revl_;      // rev-lock, held for writer's lifetime
  τ::f64         auto_f_;    // auto-repack factor (see spec)
  Ωfm            map_;
  τ::u32         mss_;       // max stage size (#elements)
  τ::u32         rev_;       // last-read revision (readers)
  τ::u32         cap_;       // max #arrays (writers)
  mutable τ::Smu as_mu_;
  τ::V<array>    as_;        // active arrays, always sorted by ascending o
  mutable τ::Smu stage_mu_;
  τ::MM<K, L>    stage_;     // staged insertions

  // NOTE: always lock stage_mu_ before as_mu_


  τ::u32 largest_array_index_ () const;
  τ::u32 smallest_array_index_() const;

  τ::u32 search_in_(array const&, Kc&, L*, τ::u32) const;

  Ωfl  lock_arrays_ (bool rw) const { return {fd_, rw, 16, 16 * cap_}; }
  bool read_header_ ();
  void write_header_(τ::u32 cap);
  void commit_      ();  // assumes stage_mu_ is locked
};


Tkl Ωh<K, L>::Ωh(τ::Stc &f, bool rw, τ::f64 auto_f, τ::u32 mss, τ::u32 cap)
  : f_     (f),
    fd_    (Ωopen(f, rw)),
    rw_    (rw),
    revl_  (fd_, rw_),
    auto_f_(auto_f),
    mss_   (mss),
    map_   (fd_, false),
    rev_   (0),
    cap_   (cap)
{
  using namespace τ;
  if (rw_)
  {
    revl_.lock(4, 4);  // claim unique write access
    if (fd_->size() < 16) write_header_(cap);
  }
  else
  {
    A(fd_->size() >= 16, "Ωh::Ωh(ro): no data in " << f);
    read_header_();
  }
}

Tkl Ωh<K, L>::~Ωh()
{
  if (rw_) commit();
}


Tkl void Ωh<K, L>::add(Kc &k, Vc &v)
{
  using namespace τ;
  Ul<Smu> l(stage_mu_);
  stage_.insert({k, v});
  if (stage_.size() > mss_) commit_();
}


Tkl τ::u32 Ωh<K, L>::get(Kc &k, V *v, τ::u32 n) const
{
  using namespace τ;
  u32 r = 0;

  {
    Sl<Smu> l(stage_mu_);
    let     e = stage_.equal_range(k);
    for (auto i = e.first; i != e.second; ++i)
      if (v && r < n) *v[r++] = i->second;
  }

  {
    // NOTE: obtain file lock first? (TODO: justify this)
    let     fl = lock_arrays_(false);
    Sl<Smu> l(as_mu_);
    for (u32 i = 0; r < n && i < as_.size(); ++i)
      r += search_in_(as_[i], k, v ? v + r : 0, n - r);
  }
  return r;
}


Tkl void Ωh<K, L>::commit()
{
  τ::Ul<τ::Smu> l(stage_mu_);
  commit_();
}


Tkl void Ωh<K, L>::repack(τ::u64 max_bytes)
{
  using namespace τ;
  Ul<Smu> l(as_mu_);

  // We always repack the smallest arrays first, so find out how many fit into
  // max_bytes and how big they would be if we combined them. Then figure out
  // whether they fit into an existing gap, or whether we should append.
  V<u32> asi(as_.size());  // indexes of as_ by ascending size
  V<u32> aoi(as_.size());  // indexes of as_ by ascending offset
  for (u32 i = 0; i < as_.size(); ++i) asi.push_back(i), aoi.push_back(i);
  std::sort(asi.begin(), asi.end(), [&](u32 a, u32 b) { return as_[a].s < as_[b].s; });
  std::sort(aoi.begin(), aoi.end(), [&](u32 a, u32 b) { return as_[a].o < as_[b].o; });

  u64 bytes = 0;
  u32 n     = 0;
  for (; n < asi.size(); ++n)
  {
    if (bytes + as_[asi[n]].s > max_bytes) break;
    bytes += as_[asi[n]].s;
  }

  // Can't repack fewer than two arrays.
  if (n < 2) return;

  // Now look for any gap large enough to accept the new array. By default we
  // append to the end of the file, but it's possible that we'll have a gap
  // large enough for the new array.
  u64 append_at = fd_->size();
  for (u32 i = 0; i + 1 < aoi.size(); ++i)
    if (as_[aoi[i + 1]].o - (as_[aoi[i]].o + as_[aoi[i]].s) >= bytes)
    {
      append_at = as_[aoi[i]].o + as_[aoi[i]].s;
      break;
    }

  V<array> ams(n);  // arrays to merge
  // TODO: array-merge machinery here (into append_at)
}


Tkl bool Ωh<K, L>::read_header_()
{
  using namespace τ;
  let h = *uap<header const>(map_ + 0);
  if (h.rev == rev_) return false;
  else
  {
    map_.update();
    as_.resize(h.n);
    for (u32 i = 0; i < as_.size(); ++i)
      as_[i] = *uap<array const>(map_ + (16 + 16*i));
    return true;
  }
}


Tkl void Ωh<K, L>::write_header_(τ::u32 cap)
{
  using namespace τ;
  header h;
  memcpy(h.magic, "Ωh\0", 4);
  h.rev = rev_;
  h.cap = cap;
  h.n   = as_.size();
  A(pwrite(fd_->fd(), &h, 16, 0) == 16,
    "Ωh::write_header_: pwrite failed");
}


#undef Tkl


}

#include "../end.hh"

#endif
