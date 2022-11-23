#ifndef τπ0asm_h
#define τπ0asm_h


#include <iterator>


#include "strings.hh"
#include "types.hh"
#include "utf9-types.hh"
#include "utf9.hh"

#include "pi0-types.hh"
#include "pi0-int.hh"
#include "pi0-gc.hh"
#include "pi0-abi.hh"
#include "pi0-pgm.hh"

#include "begin.hh"

namespace τ
{


#if τdebug_iostream
struct π0afr;
O &operator<<(O &, π0afr const&);
#endif


struct π0afr  // π₀ asm frame
{
  sletc  c7fs = cs7(" |[]");
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


struct π0asm
{
  sletc c7ws = cs7(" \t\n");           // whitespace
  sletc c7ni = cs7(" \t\n[]|'\\\"#");  // non-ident
  sletc c7nc = cs7(" \t\n[]'");        // non-[ident]-continuation
  sletc c7sc = cs7("(){},");           // single-char idents
  sletc c7in = cs7("0123456789");      // integer

  typedef V<π0b> π0blk;  // code block

  π0abi const &a;
  M<St, π0fi>  fn;  // string index of ABI functions
  B            qh;  // static quoted values
  V<π0afr>     fs;  // stack of frames
  V<π0blk>     bs;  // stack of blocks

  π0asm(π0abi const &a_) : a(a_)
  { fs.push_back(π0afr{});
    bs.push_back(π0blk{});
    for (uN i = 0; i < a.f.size(); ++i) fn[a.n.at(i)] = i; }


  π0asm &begin() { bs.push_back(π0blk{}); return *this; }
  π0asm &end()
  { auto b = std::move(bs.back()); bs.pop_back();
    *this << f("[", b.size() + 1);
    for (let &x : b) *this << x;
    *this << f("]", 0);
    return *this; }

  π0asm &fbegin(Stc &vs)
  { fs.push_back(π0afr(vs));
    *this << f("[|", fs.back().vs.size());
    return *this; }

  π0asm &fend()
  { fs.pop_back();
    *this << f("|]");
    return *this; }


  π0b f(Stc &n, uN k = 0)
    { A(!n.empty(), "π₀asm f(\"\")");
      if (!fs.empty())
      { let a = fs.back()[n];
        if (a != π0afr::fω) return π0b{fn.at("&@'"), a};
        if (n.ends_with('='))
        { let b = fs.back()[n.substr(0, n.size() - 1)];
          if (b != π0afr::fω) return π0b{fn.at("&='"), b}; } }
      if (fn.contains(n)) return π0b{fn.at(n), k};
      if (c7in[n[0]])
      { std::stringstream s{n};
        i64 x;
        s >> x;
        A(!oi<i32>(x), "int literal overflow: " << x << " from " << n);
        return π0b{fn.at(oi<i16>(x) ? "i32" : oi<i8>(x) ? "i16" : "i8"), x}; }
      A(0, "π₀asm: " << n << " is not defined"); }


  π0asm &operator<<(π0b b) { bs.back().push_back(b); return *this; }
  π0asm &operator<<(Stc &s)
  { for (uN i = 0; i < s.size(); ++i)
      if      (c7ws[s[i]]) continue;
      else if (c7sc[s[i]]) *this << f(s.substr(i, 1), 0);
      else if (c7ni[s[i]])
        switch (s[i])
        {
        case '#': while (i < s.size() && s[i] != '\n') ++i; break;
        case '[': begin(); break;
        case ']': end();   break;
        case '\'':
        { uN j = i + 1;
          while (j < s.size() && !c7ni[s[j]]) ++j;
          *this << f("sym", qh << o9(u9_symbol::str(s.substr(i + 1, j - i - 1))));
          i = j - 1;
          break; }
        case '"':
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
          *this << f("utf8", qh << o9(x));
          break; }
        case '|':
        { if (i + 1 < s.size() && s[i + 1] == ']') { fend(); end(); ++i; }
          else
          { uN j = i + 1;
            while (j < s.size() && s[j] != '|') ++j;
            fbegin(s.substr(i + 1, j - i - 1));
            *this << f("[|", fs.back().vs.size());
            i = j; }
          break; }
        TA(*this, "π₀asm<< ni " << s[i]);
        }
      else
      { uN j = i + 1;
        while (j < s.size() && !c7nc[s[j]]) ++j;
        let n = s.substr(i, j - i);
        uN  x = 0;
        if (j < s.size() && s[j] == '\'')
          for (i = j++; j < s.size() && c7in[s[j]]; ++j)
            x *= 10, x += s[j] - '0';
        *this << f(n, x);
        i = j - 1; }
    return *this; }


  SP<π0pgm> build()
  { A(bs.size() == 1, "π₀asm::build |bs| = " << bs.size());
    *this << f("]", 0);
    return SP<π0pgm>(new π0pgm{a, qh, bs.back()}); }
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

#include "end.hh"


#endif
