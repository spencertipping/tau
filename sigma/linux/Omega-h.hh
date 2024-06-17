#ifndef σserver_Ωh_h
#define σserver_Ωh_h

#include "Omega-io.hh"
#include "../begin.hh"

namespace σ
{


#define Tkv template<class K, class V>


Tkv struct Ωh final
{
  typedef K const Kc;
  typedef V const Vc;

  Ωh(τ::Stc&, bool rw = false, τ::f64 auto_f = 1.5, τ::u32 cap = 255);

  void   add     (Kc&, Vc&);
  τ::u32 get     (Kc&, V*, τ::u32) const;
  void   commit  ();        // commit the stage to a new array
  void   repack  (τ::u64);  // repack up to this many bytes
  τ::u64 unpacked() const;

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

  τ::Stc      f_;
  Ωfd         fd_;
  bool        rw_;
  Ωfl         revl_;
  τ::f64      auto_f_;    // auto-repack factor (see spec)
  Ωfm         map_;
  τ::u32      rev_;
  τ::u32      cap_;
  τ::V<array> as_;        // active arrays, always sorted by ascending o
  τ::Smu      stage_mu_;
  τ::MM<K, V> stage_;

  τ::u32 largest_array_index_ () const;
  τ::u32 smallest_array_index_() const;

  Ωfl  lock_arrays_ (bool rw);
  bool read_header_ ();
  void write_header_(τ::u32 cap);
};


Tkv Ωh<K, V>::Ωh(τ::Stc &f, bool rw, τ::f64 auto_f, τ::u32 cap)
  : f_     (f),
    fd_    (Ωopen(f, rw)),
    rw_    (rw),
    revl_  (fd_, rw_),
    auto_f_(auto_f),
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


Tkv bool Ωh<K, V>::read_header_()
{
  using namespace τ;
  let h = *uap<header const>(map_ + 0);
  // TODO
  //
  // if (h.rev != rev_)
  //   // reload arrays
}


Tkv void Ωh<K, V>::write_header_(τ::u32 cap)
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


#undef Tkv


}

#include "../end.hh"

#endif
