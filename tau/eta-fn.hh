#ifndef τηfn_h
#define τηfn_h

#include "types.hh"
#include "eta-types.hh"
#include "eta-auto.hh"

#include "begin.hh"

namespace τ
{


template<class... Xs> struct ηfn_ts
{
  sletc ts = ηtl{ηauto_<Xs>::t...};
};


}

#include "end.hh"

#endif
