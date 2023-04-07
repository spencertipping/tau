#ifndef τπfn_η_h
#define τπfn_η_h


#include "pi-fn.hh"

#include "begin.hh"

namespace τ
{


template<class T>
struct πbcast
{
  T x;
  T     operator[](uN i) const { return x; }
  std::size_t size()     const { return Nl<std::size_t>::max(); }
};

template<class T> πbcast(T) -> πbcast<T>;


πdf πηadd();
πdf πηsub();
πdf πηmul();
πdf πηdiv();
πdf πηmod();

πdf πηlsh();
πdf πηrsh();
πdf πηand();
πdf πηor();
πdf πηxor();

πdf πηlt();
πdf πηle();
πdf πηgt();
πdf πηge();
πdf πηeq();
πdf πηne();

πdf πηvlt();
πdf πηvle();
πdf πηvgt();
πdf πηvge();
πdf πηveq();
πdf πηvne();


}

#include "end.hh"


#endif
