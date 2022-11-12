#ifndef τstrings_h
#define τstrings_h


#include <algorithm>
#include <string>


#include "types.hh"


#include "begin.hh"

namespace τ
{


#if τdebug_iostream
struct cs7;
O &operator<<(O&, cs7 const&);
#endif


inline Bv sBv(chc *x) { return Bv{Rc<u8c*>(x)}; }


struct cs7  // 7-bit ASCII char set, used to split things
{
  u64 c1;
  u64 c2;

  constexpr cs7(chc *cs)
    : c1(0), c2(0)
    { for (ch c; c = *cs++;)
        if (c >= 0 && c < 64) c1 |= 1ull << c;
        else if (c >= 64)     c2 |= 1ull << c - 64; }

  bool operator[](ch c) const
    { return c >= 0 && (c < 64 ? c1 & 1ull << c : c2 & 1ull << c - 64); }

  V<St> split(Stc &s) const
    { V<St> r;
      for (uN i = 0; i < s.size(); ++i)
      { while (i < s.size() && (*this)[s[i]]) ++i;
        uN j = i;
        while (j < s.size() && !(*this)[s[j]]) ++j;
        r.push_back(s.substr(i, j - i));
        i = j - 1; }
      return r; }
};


ic bool ieq(Bv const &a, Bv const &b)
{
  return std::equal(a.begin(), a.end(),
                    b.begin(), b.end(),
                    [](u8 x, u8 y) { return std::tolower(x) == std::tolower(y); });
}


#if τdebug_iostream
O &operator<<(O &s, cs7 const &c)
{
  TODO("<<cs7");
}
#endif


}

#include "end.hh"


#endif
