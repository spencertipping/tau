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

#include "pi0-begin.hh"

namespace τ
{


struct π0cs7  // 7-bit ASCII charset
{
  u64 c1;
  u64 c2;

  constexpr π0cs7(chc *cs)
    : c1(0), c2(0)
    { for (ch c; c = *cs; cs++)
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
  sletc c7fs = π0cs7(" |[]");

  sc uNc fω = -1;
  V<St>  vs;

  π0afr(Stc &vs_) : vs(c7fs.split(vs_)) {}
  π0afr(Il<St> const &vs_)
    { std::copy(vs_.begin(), vs_.end(), std::back_inserter(vs)); }
  π0afr(Stc &p, uN vs_)
    { for (uN i = 0; i < vs_; ++i)
      { vs.push_back(p);
        vs.back().append(std::to_string(i)); } }

  uN operator[](Stc &v)
    { for (uN i = 0; i < vs.size(); ++i) if (vs[i] == v) return i;
      return fω; }
};


struct π0abl  // π₀ asm block
{
  V<uN> is;  // uncompressed instructions
  π0abl &operator<<(uN i) { is.push_back(i); return *this; }
};


π0TGs π0asm
{
  π0TS;
  sletc c7ws = π0cs7(" \t\n");         // whitespace
  sletc c7ni = π0cs7(" \t\n{}[](),");  // non-ident
  sletc c7in = π0cs7("0123456789");    // integer


  π0T(π0abi)  a;
  M<St, π0fi> fn;  // string index of ABI functions


  M<uN, uN> gf;   // local getters (local index → fs[i])
  M<uN, uN> sf;   // local setters (local index → fs[i])
  V<π0f>    fs;   // quotation and defined functions (not all functions are named)
  B         qh;   // static quoted-value heap
  V<π0abl*> bls;  // stack of blocks
  V<π0afr*> frs;  // stack of frames
  uN        ret;  // index of return function


  π0asm()
    { ret = def([](π0int &i) { i.r.pop_back(); });
      begin(); }


  π0abl &cb() { return *bls.back(); }
  π0afr &cf() { return *frs.back(); }


  uN getf(Stc &n)
    { let i = cf()[n];
      if (!gf.contains(i)) gf[i] = def([i](π0int &z) { z.h.dpush(z.h.fi(i)); });
      return gf[i]; }

  uN setf(Stc &n)
    { let i = cf()[n.back() == '=' ? n.substr(0, n.size() - 1) : n];
      if (!sf.contains(i)) sf[i] = def([i](π0int &z) { z.h.fs(i, z.h.di(0)); z.h.dpop(); });
      return sf[i]; }


  template<class T>
  π0asm &def(Stc &n, T &&f)
    { A(!fn.contains(n), "π0asm redef " << n);
      fn[n] = def(std::move(f));
      return *this; }

  uN def(π0f &&f) { fs.push_back(std::move(f)); return fs.size() - 1; }

  uN def(F<void(π0int&, i9)> &&f)
    { return def([f = std::move(f)](π0int &z)
      { let a = z.h.di(0);
        z.h.dpop();
        f(z, a); }); }

  uN def(F<void(π0int&, i9, i9)> &&f)
    { return def([f = std::move(f)](π0int &z)
      { let a = z.h.di(0);
        let b = z.h.di(1);
        z.h.dpop(2);
        f(z, a, b); }); }

  uN def(F<void(π0int&, i9, i9, i9)> &&f)
    { return def([f = std::move(f)](π0int &z)
      { let a = z.h.di(0);
        let b = z.h.di(1);
        let c = z.h.di(2);
        z.h.dpop(3);
        f(z, a, b, c); }); }


  π0asm &begin() { bls.push_back(new π0abl()); return *this; }
  π0asm &end(bool r = true)
    { let b = bls.back(); bls.pop_back();
      cb() << def([n = b->is.size() + r](π0int &i)
        { i << i.r.back();
          i.r.back() += n; });
      for (let i : b->is) cb() << i;
      delete b;
      if (r) cb() << ret;
      return *this; }

  // TODO: add [|a= b= c d| ...] syntax to capture some locals from the
  // stack when the frame is initialized
  π0asm &frame(Stc &vs) { TODO("π0asm frame"); }
  π0asm &fpop ()        { TODO("π0asm fpop"); }


  template<o9mapped T>
  π0asm &l(T const &x)
    { let i = qh << o9(x);
      cb() << def([i](π0int &z) { z.h.dpush(o9(z[i])); });
      return *this; }

  π0asm &f(Stc &c)
    { if (fn.contains(c))  { cb() << fn.at(c); return *this; }
      if (c.back() == '=') { cb() << setf(c);  return *this; }
      cb() << getf(c);
      return *this; }


  π0asm &operator<<(Stc &s)
    { for (uN i = 0; i < s.size(); ++i)
        if      (c7ws[s[i]]) continue;
        else if (c7ni[s[i]])
        { if      (s[i] == '[') begin();
          else if (s[i] == ']') end();
          else if (s[i] == '|') TODO("<< frame");
          else A(0, "π₀asm<< internal error " << s[i]); }
        else if (c7in[s[i]])
        { uN j = i + 1;
          while (j < s.size() && c7in[s[j]]) ++j;
          let n = s.substr(i, j - i);
          l(atoi(n.c_str()));
          i = j - 1; }
        else
        { uN j = i + 1;
          while (j < s.size() && !c7ni[s[j]]) ++j;
          f(s.substr(i, j - i));
          i = j - 1; }

      return *this; }


  // FIXME: this should not invalidate the assembler
  π0int build()
    { A(bls.size() == 1, "π₀ bs=" << bls.size() << " ≠ 1");
      cb() << ret;
      return π0int(qh, std::move(fs), std::move(cb().is), 0); }
};


#if τdebug_iostream
O &operator<<(O &s, π0afr const &f)
{
  s << "| ";
  for (let &v : f.vs) s << v << " ";
  return s << "|";
}

O &operator<<(O &s, π0abl const &b)
{
  if (b.is.empty()) return s << "[]";
  s << "[" << b.is[0];
  for (uN i = 1; i < b.is.size(); ++i) s << " " << b.is[i];
  return s << "]";
}
#endif


}

#include "pi0-end.hh"


#endif
