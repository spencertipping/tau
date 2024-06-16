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

  Ωh(τ::Stc&, bool rw = false);

  void   add     (Kc&, Vc&);
  τ::u32 get     (Kc&, V*, τ::u32) const;
  void   commit  ();        // commit the stage to a new array
  void   repack  (τ::u64);  // repack up to this many bytes
  τ::u64 unpacked() const;

protected:
  struct array final
  {
    τ::u64 o;
    τ::u64 s;
  };

  τ::Stc      f_;
  Ωfd         fd_;
  bool        rw_;
  τ::f64      auto_f_;    // auto-repack factor (see spec)
  Ωfm         map_;
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


#undef Tkv


}

#include "../end.hh"

#endif
