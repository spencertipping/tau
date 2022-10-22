#ifndef τπ0asm_h
#define τπ0asm_h


#include <iterator>


#include "types.hh"
#include "utf9-types.hh"
#include "utf9.hh"

#include "pi0-types.hh"
#include "pi0-int.hh"
#include "pi0-gc.hh"
#include "pi0-abi.hh"
#include "pi0-pgm.hh"

#include "pi0-begin.hh"

namespace τ
{


struct π0cs7  // 7-bit ASCII char set, used to split things
{
  u64 c1;
  u64 c2;

  constexpr π0cs7(chc *cs)
    : c1(0), c2(0)
    { for (ch c; c = *cs++;)
        if (c >= 0 && c < 64) c1 |= 1ull << c;
        else if (c >= 64)     c2 |= 1ull << c - 64; }

  bool operator[](ch c) const
    { return c >= 0 && (c < 64 ? c1 & 1ull << c : c2 & 1ull << c - 64); }

  V<St> split(Stc &s) const
    { V<St> r;
      if (s.empty()) return r;
      bool c = false;
      uN   k = 0;
      for (uN i = 0; i < s.size(); ++i)
      { let b = (*this)[s[i]];
        if      (b  && !c) k = i;
        else if (!b &&  c) r.push_back(s.substr(k, i - k));
        c = b; }
      if (c) r.push_back(s.substr(k));
      return r; }
};


struct π0afr  // π₀ asm frame
{
  sletc  c7fs = π0cs7(" |[]");
  sc uNc fω   = -1;

  V<St> vs;   // variables, positionally mapped to frame offsets
  uN    nc;   // number of arg-captures when entering frame

  π0afr() {}
  π0afr(π0afr &&a) : vs(std::move(a.vs)), nc(a.nc) {}

  π0afr(Stc &vs_) : vs(c7fs.split(vs_)) {}
  π0afr(Il<St> const &vs_)
    { std::copy(vs_.begin(), vs_.end(), std::back_inserter(vs)); }

  π0afr(Stc &p, uN n)  // anonymous variables with prefix
    { for (uN i = 0; i < n; ++i)
      { vs.push_back(p);
        vs.back().append(std::to_string(i)); } }

  π0afr &operator=(π0afr const &a)
    { vs = a.vs;
      nc = a.nc;
      return *this; }

  uN operator[](Stc &v)
    { for (uN i = 0; i < vs.size(); ++i) if (vs[i] == v) return i;
      return fω; }
};


π0TGs π0asm
{
  π0TS;
  sletc c7ws = π0cs7(" \t\n");            // whitespace
  sletc c7ni = π0cs7(" \t\n{}[](),'\"");  // non-ident
  sletc c7in = π0cs7("0123456789");       // integer

  typedef V<π0b> π0blk;  // code block

  π0T(π0abi) const &a;
  M<St, π0fi>       fn;  // string index of ABI functions
  B                 qh;  // static quoted values
  V<π0afr>          fs;  // stack of frames
  V<π0blk>          bs;  // stack of blocks

  π0asm(π0T(π0abi) const &a_) : a(a_)
  { fs.push_back(π0afr{});
    bs.push_back(π0blk{});
    for (uN i = 0; i < a.f.size(); ++i) fn[a.n.at(i)] = i; }


  π0T(π0asm) &begin() { bs.push_back(π0blk{}); return *this; }
  π0T(π0asm) &end()
  { auto b = std::move(bs.back()); bs.pop_back();
    *this << π0b{f("["), b.size() + 1};
    for (let &x : b) *this << x;
    *this << π0b{f("]"), 0};
    return *this; }

  π0T(π0asm) &fbegin(Stc &vs)
  { fs.push_back(π0afr(vs));
    *this << π0b{f("[|"), fs.back().vs.size() << 16 | fs.back().nc};
    return *this; }

  π0T(π0asm) &fend()
  { fs.pop_back();
    *this << π0b{f("|]"), 0};
    return *this; }


  uN f(Stc &n)
  { A(fn.contains(n), "π₀asm: " << n << " is not defined");
    return fn.at(n); }


  π0T(π0asm) &operator<<(π0b b) { bs.back().push_back(b); return *this; }
  π0T(π0asm) &operator<<(Stc &s)
  { for (uN i = 0; i < s.size(); ++i)
      if      (c7ws[s[i]]) continue;
      else if (c7ni[s[i]])
      { if      (s[i] == '[') begin();
        else if (s[i] == ']') end();
        else if (s[i] == '\'')
        { uN j = i + 1;
          while (j < s.size() && !c7ni[s[j]]) ++j;
          *this << π0b{f("sym"), qh << o9(u9_symbol::str(s.substr(i, j - 1)))};
          i = j - 1; }
        else if (s[i] == '"')
        { St x;
          for (++i; i < s.size() && s[i] != '"'; ++i)
            if (s[i] == '\\' && ++i < s.size())
              switch (s[i])
              {
              case '"': case '\\': x.push_back(s[i]); break;
              case 'n': x.push_back('\n'); break;
              case 't': x.push_back('\t'); break;
              case 'r': x.push_back('\r'); break;
                TA(*this, "π₀asm unknown string escape: " << s[i]);
              }
            else x.push_back(s[i]);
          *this << π0b{f("utf8"), qh << o9(x)}; }
        else if (s[i] == '|') TODO("<< frame");
        else A(0, "π₀asm<< internal error " << s[i]); }
      else
      { uN j = i + 1;
        while (j < s.size() && !c7ni[s[j]]) ++j;
        let n = s.substr(i, j - i);
        uN  x = 0;
        if (j < s.size() && s[j] == '\'')
          for (i = j++; j < s.size() && c7in[s[j]]; ++j)
            x *= 10, x += s[j] - '0';
        *this << π0b{f(n), x};
        i = j - 1; }
    return *this; }


  π0T(π0p) build()
  { A(bs.size() == 1, "π₀asm::build |bs| = " << bs.size());
    *this << π0b{f("]"), 0};
    return π0T(π0p){a, qh, bs.back()}; }
};


#if τdebug_iostream
O &operator<<(O &s, π0afr const &f)
{
  s << "| ";
  for (let &v : f.vs) s << v << " ";
  return s << "|";
}
#endif


}

#include "pi0-end.hh"


#endif
