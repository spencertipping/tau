#pragma once


#include "utf9/errors.hh"
#include "utf9/numerics.hh"
#include "utf9/hash.hh"
#include "utf9/ibuf.hh"
#include "utf9/obuf.hh"

#include "utf9/tpack.hh"
#include "utf9/lfn.hh"
#include "utf9/pfn.hh"
#include "utf9/typecode.hh"
#include "utf9/primitive.hh"

#include "utf9/tval.hh"
#include "utf9/val.hh"
#include "utf9/valsearch.hh"
#include "utf9/enc.hh"
#include "utf9/ostream.hh"


namespace tau::utf9
{

inline void utf9_init()
{
  init_lfn_tables();
  init_sfn_tables();
}

}


#define U9 (tau::utf9::val)
