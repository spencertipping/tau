#include "pi-val.hh"
#include "begin.hh"

namespace τ
{


sc ηtype πvts[] =
{
  ηtype::invalid,
  ηtype::η,
  ηtype::η,
  ηtype::int8s,
  ηtype::int16s,
  ηtype::int32s,
  ηtype::int64s,
  ηtype::float32s,
  ηtype::float64s,

  ηtype::sig,
  ηtype::n_int,
  ηtype::n_float,
  ηtype::string,
  ηtype::atom,
  ηtype::η,
  ηtype::int8s,
  ηtype::int16s,
  ηtype::int32s,
  ηtype::int64s,
  ηtype::float32s,
  ηtype::float64s
};


ηtype πvt(πvc &x)
{
  return πvts[x.index()];
}


}

#include "end.hh"
