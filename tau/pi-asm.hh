#ifndef τπasm_h
#define τπasm_h


#include <iterator>


#include "strings.hh"
#include "types.hh"
#include "utf9-types.hh"
#include "utf9.hh"

#include "pi-types.hh"
#include "pi-int.hh"
#include "pi-gc.hh"
#include "pi-abi.hh"
#include "pi-pgm.hh"

#include "begin.hh"

namespace τ
{


#if τdebug_iostream
struct πafr;
O &operator<<(O &, πafr const&);
#endif


struct πafr  // π₀ asm frame
{
  sletc  c7fs = cs7(" |[]");
  sc uNc fω   = -1;

  V<St> vs;   // variables, positionally mapped to frame offsets
  uN    nc;   // number of arg-captures when entering frame

  πafr() {}
  πafr(πafr &&a) : vs(std::move(a.vs)), nc(a.nc) {}

  πafr(Stc &vs_) : vs(c7fs.split(vs_)) {}
  πafr(Il<St> const &vs_)
    { std::copy(vs_.begin(), vs_.end(), std::back_inserter(vs)); }

  πafr(Stc &p, uN n)  // anonymous variables with prefix
    { for (uN i = 0; i < n; ++i)
      { vs.push_back(p);
        vs.back().append(std::to_string(i)); } }

  πafr &operator=(πafr const &a)
    { vs = a.vs;
      nc = a.nc;
      return *this; }

  uN operator[](Stc &v)
    { for (uN i = 0; i < vs.size(); ++i) if (vs[i] == v) return i;
      return fω; }
};


struct πasm
{
  sletc c7ws = cs7(" \t\n");           // whitespace
  sletc c7ni = cs7(" \t\n[]|'\\\"#");  // non-ident
  sletc c7nc = cs7(" \t\n[]'");        // non-[ident]-continuation
  sletc c7sc = cs7("(){},");           // single-char idents
  sletc c7in = cs7("0123456789");      // integer

  typedef V<πb> πblk;  // code block

  πabi const &a;
  M<St, πfi>       fn;   // string index of ABI functions
  M<u9_symbol, uN> qhi;  // quoted-symbol offset index (within qh)
  B                qh;   // static quoted values
  V<πafr>          fs;   // stack of frames
  V<πblk>          bs;   // stack of blocks

  πasm(πabi const &a_) : a(a_)
  { fs.push_back(πafr{});
    bs.push_back(πblk{});
    bs.push_back(πblk{});
    for (uN i = 0; i < a.f.size(); ++i) fn[a.n.at(i)] = i; }


  πasm &begin() { bs.push_back(πblk{}); return *this; }
  πasm &end()
    { auto b = std::move(bs.back()); bs.pop_back();
      f("[", b.size() + 1);
      for (let &x : b) *this << x;
      f("]");
      return *this; }

  πasm &iend()  // inline-end: don't quote the block
    { auto b = std::move(bs.back()); bs.pop_back();
      for (let &x : b) *this << x;
      f("]");
      return *this; }


  πasm &fbegin(Stc &vs)
  { fs.push_back(πafr(vs));
    f("[|", fs.back().vs.size());
    return *this; }

  πasm &fend()
  { fs.pop_back();
    f("|]");
    return *this; }


  uN qs(u9_symbol const &k)
    { if (!qhi.contains(k)) qhi[k] = qh << o9(k);
      return qhi.at(k); }


  πasm &f(Stc &n, uN k = 0)
    { A(!n.empty(), "π₀asm f(\"\")");
      if (!fs.empty())
      { let a = fs.back()[n];
        if (a != πafr::fω) return *this << πb{fn.at("&@'"), a};
        if (n.ends_with('='))
        { let b = fs.back()[n.substr(0, n.size() - 1)];
          if (b != πafr::fω) return *this << πb{fn.at("&='"), b}; } }
      if (fn.contains(n)) return *this << πb{fn.at(n), k};
      if (c7in[n[0]])
      { i64 x = 0;
        if (n.starts_with("0x"))
          for (uN i = 2; i < n.size(); ++i)
            x = x << 4 | St{"0123456789abcdef"}.find(n[i]);
        else
          std::stringstream{n} >> x;
        A(!oi<i32>(x), "int literal overflow: " << x << " from " << n);
        return *this << πb{fn.at(oi<i16>(x) ? "i32" : oi<i8>(x) ? "i16" : "i8"), x}; }

      // Otherwise, interpret the name as a global _function_ that is
      // expected to exist by this point.
      A(fn.contains("sym"), "no sym literal fn in π₀asm (infinite loop)");
      A(fn.contains("%@"),  "no %@ fn in π₀asm (infinite loop)");
      A(fn.contains("."),   "no . fn in π₀asm (infinite loop)");

      f("sym", qs(u9_symbol::str(n)));
      f("%@");
      f(".");
      return *this; }


  πasm &operator<<(πb b) { bs.back().push_back(b); return *this; }
  πasm &operator<<(Stc &s)
  { for (uN i = 0; i < s.size(); ++i)
      if      (c7ws[s[i]]) continue;
      else if (c7sc[s[i]]) f(s.substr(i, 1), 0);
      else if (c7ni[s[i]])
        switch (s[i])
        {
        case '#': while (i < s.size() && s[i] != '\n') ++i; break;
        case '[': begin(); break;
        case ']': end();   break;
        case '\'':
        { uN j = i + 1;
          while (j < s.size() && !c7ni[s[j]]) ++j;
          f("sym", qs(u9_symbol::str(s.substr(i + 1, j - i - 1))));
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
          f("utf8", qh << o9(x));
          break; }
        case '|':
        { if (i + 1 < s.size() && s[i + 1] == ']') { fend(); end(); ++i; }
          else
          { uN j = i + 1;
            while (j < s.size() && s[j] != '|') ++j;
            fbegin(s.substr(i + 1, j - i - 1));
            i = j; }
          break; }
        TA(*this, "π₀asm<< ni " << s[i]);
        }
      else
      { uN j = i + 1;
        while (j < s.size() && !c7nc[s[j]] && !c7sc[s[j]]) ++j;
        let n = s.substr(i, j - i);
        uN  x = 0;
        if (j < s.size() && s[j] == '\'')
          for (i = j++; j < s.size() && c7in[s[j]]; ++j)
            x *= 10, x += s[j] - '0';
        f(n, x);
        i = j - 1; }
    return *this; }


  SP<πpgm> build_repl()
    { A(bs.size() == 2, "π₀asm::build_repl |bs| = " << bs.size());
      let e = Sc<πbi>(bs[0].size());
      iend();
      let r = SP<πpgm>(new πpgm{a, qh, bs.back(), e});
      begin();
      return r; }

  SP<πpgm> build()
    { A(bs.size() == 2, "π₀asm::build |bs| = " << bs.size());
      f("]");
      return SP<πpgm>(new πpgm{a, qh, bs.back(), 0}); }
};


#if τdebug_iostream
O &operator<<(O &s, πafr const &f)
{
  s << "| ";
  for (let &v : f.vs) s << v << " ";
  return s << "|";
}
#endif


}

#include "end.hh"


#endif
