#ifndef τstrings_h
#define τstrings_h


#include <algorithm>
#include <string>


#include "types.hh"


#include "begin.hh"

namespace τ
{


struct cs7  // 7-bit ASCII char set, used to split things
{
  constexpr cs7(u64 c1_, u64 c2_) : c1(c1_), c2(c2_) {}
  constexpr cs7(chc *cs)
    : c1(0), c2(0)
    { for (ch c; c = *cs++;)
        if (c >= 0 && c < 64) c1 |= 1ull << c;
        else if (c >= 64)     c2 |= 1ull << c - 64; }

  bool operator[](ch c) const
    { return c >= 0 && (c < 64 ? c1 & 1ull << c : c2 & 1ull << c - 64); }

  V<uN> &find(Stvc &s, V<uN> &r) const
    { for (uN i = 0; i < s.size(); ++i)
        if ((*this)[s[i]]) r.push_back(i);
      return r; }

  constexpr cs7 operator|(cs7 const &x) const { return {c1 | x.c1, c2 | x.c2}; }
  constexpr cs7 operator&(cs7 const &x) const { return {c1 & x.c1, c2 & x.c2}; }
  constexpr cs7 operator^(cs7 const &x) const { return {c1 ^ x.c1, c2 ^ x.c2}; }
  constexpr cs7 operator~()             const { return {~c1, ~c2}; }

protected:
  u64 c1;
  u64 c2;
};


O &operator<<(O &s, cs7 const &c);


}

#include "end.hh"


#endif
