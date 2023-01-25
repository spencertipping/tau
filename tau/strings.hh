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

  V<St> split(Stc &s) const  // TODO: rewrite in terms of find()
    { V<St> r;
      for (uN i = 0; i < s.size(); ++i)
      { while (i < s.size() && (*this)[s[i]]) ++i;
        uN j = i;
        while (j < s.size() && !(*this)[s[j]]) ++j;
        r.push_back(s.substr(i, j - i));
        i = j - 1; }
      return r; }

  V<uN> find(Stc &s) const
    { V<uN> r;
      for (uN i = 0; i < s.size(); ++i)
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


#if τdebug_iostream
O &operator<<(O &s, cs7 const &c)
{
  s << "cs7[";
  for (u8 i = 0; i < 128; ++i) if (c[i]) s << i;
  return s << "]";
}
#endif


}

#include "end.hh"


#endif
