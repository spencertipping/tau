#pragma once


#include <cstdint>
#include <stdexcept>


typedef int32_t tpacked;

void pack_table(uint64_t const *       t,
                uint64_t const         base,
                tpacked        * const p,
                int              const n)
{
  for (int i = 0; i < n; ++i)
  { p[i] = static_cast<tpacked>(t[i] - base);
    if (p[i] + base != t[i]) throw std::length_error("pack_table overflow"); }
}
