#include <zstd.h>

#include "zstd.hh"
#include "../begin.hh"

namespace σ
{


void Γzstd(Γφ &g)
{
  g .pi()
    .def_sr(">Z", [](ηic &x, πP<i64> l)
      {
        ηbin r; r.x.resize(ZSTD_compressBound(x.size()));
        let n = ZSTD_compress(r.x.data(), r.x.size(), x.data(), x.size(), l.x);
        r.x.resize(ZSTD_isError(n) ? 0 : n);
        return r;
      })
    .def_sr("<Z", [](ηic &x)
      {
        ηbin r; r.x.resize(ZSTD_findFrameCompressedSize(x.data(), x.size()));
        let n = ZSTD_decompress(r.x.data(), r.x.size(), x.data(), x.size());
        r.x.resize(ZSTD_isError(n) ? 0 : n);
        return r;
      });
}


}

#include "../end.hh"
