#include "ctypes.hh"

#define XXH_INLINE_ALL
#include "../dep/xxhash.h"


#include "begin.hh"


template<> struct std::hash<τ::B>
{
  size_t operator()(τ::B const &x) const
    { return XXH64(x.data(), x.size(), 0); }
};


#include "end.hh"
