#ifndef σserver_Ω_h
#define σserver_Ω_h

#include <xxhash.h>

#include "../prof.hh"
#include "../begin.hh"

namespace σ
{


void ΓΩ(Γφ&);


struct Ωfl final
{
  Ωfl (τ::fd_t, bool rw = false);
  ~Ωfl();

protected:
  τ::fd_t fd_;
  bool    rw_;
};


struct Ωf final
{
  Ωf (τ::Stc&, bool rw = false);
  ~Ωf();

  Ωf   &operator=(τ::ηic&);
  τ::ηm operator*() const;

protected:
  τ::Stc  f_;
  τ::fd_t fd_;
  bool    rw_;
};


struct Ωl final
{
  Ωl (τ::Stc&, bool rw = false);
  ~Ωl();

  typedef τ::u64 key;

  key    operator<<(τ::ηic&);
  τ::ηi  operator[](key) const;  // memory-mapped (for smaller values)
  τ::ηm  get       (key) const;  // bulk-loaded (for larger values)
  key    next      (key) const;
  τ::u64 offset    (key) const;
  τ::u64 size      (key) const;

protected:
  τ::Stc      f_;
  τ::fd_t     fd_;
  bool        rw_;
  void const *map_;
};


template<class K, class V> struct Ωh final
{
  typedef K const Kc;
  typedef V const Vc;

  Ωh (τ::Stc&, bool rw = false);
  ~Ωh();

  void   add   (Kc&, Vc&);
  τ::u32 get   (Kc&, V*, τ::u32) const;
  void   commit();        // commit the stage to a new array
  void   repack(τ::f64);  // repack up to this fraction of values

protected:
  struct array final
  {
    τ::u64 o;
    τ::u64 s;
  };

  τ::Stc      f_;
  τ::fd_t     fd_;
  bool        rw_;
  void const *map_;
  τ::u32      rev_;
  τ::u32      cap_;
  τ::V<array> as_;        // active arrays, always sorted by ascending o
  τ::Smu      stage_mu_;
  τ::MM<K, V> stage_;

  τ::u32 largest_array_ () const;
  τ::u32 smallest_array_() const;

  // TODO: find gaps

  Ωfl lock_arrays_(bool rw);
};


}

#include "../end.hh"

#endif
