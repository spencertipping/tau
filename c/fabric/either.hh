#ifndef tau_fabric_either_h
#define tau_fabric_either_h


#include <cassert>
#include <iostream>


#include "../begin.hh"

namespace tau::fabric
{


template<class L, class R>
struct either
{
  bool const is_l;
  union { L l_; R r_; } const;

  L const  &l()        const { return static_cast<L&>(*this); }
  R const  &r()        const { return static_cast<R&>(*this); }
  operator L const &() const { assert(is_l);  return l_; }
  operator R const &() const { assert(!is_l); return r_; }

  L const &l_or(L const &l__) const { return is_l ? l_  : l__; }
  R const &r_or(R const &r__) const { return is_l ? r__ : r_ ; }
};


template<class L, class R>
either<L, R> left(L const &l) { return either<L, R>{true, l}; }

template<class L, class R>
either<L, R> right(R const &r) { return either<L, R>{true, r}; }


template<class L, class R>
static std::ostream &operator<<(std::ostream &s, either<L, R> &e)
{
  if (e.is_l) return s << "left[" << e.l << "]";
  else        return s << "right[" << e.r << "]";
}


}


#include "../end.hh"

#endif
