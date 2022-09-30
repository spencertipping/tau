#ifndef τπ0asm_h
#define τπ0asm_h


#include <iterator>


#include "types.hh"
#include "utf9-types.hh"
#include "utf9.hh"

#include "pi0-types.hh"
#include "pi0-int.hh"
#include "pi0-gc.hh"

#include "begin.hh"

namespace τ
{


struct π0asm
{
  struct frame
  {
    V<St> vs;

    frame(St vs_)
      { St                x;
        std::stringstream s(vs_);
        while (s >> x) vs.push_back(x); }

    frame(Il<St> vs_)
      { std::copy(vs_.begin(), vs_.end(), std::back_inserter(vs)); }

    uN operator[](St v)
      { for (uN i = 0; i < vs.size(); ++i) if (vs[i] == v) return i;
        return -1; }
  };

  enum class iop
  {
    quote_utf9,
    quote_fn,
    call_fn
  };

  struct insn
  {
    iop o;
    i9 u;
    uN b;
    St f;

    insn()     :                     u{ζωp}       {}
    insn(i9 x) : o(iop::quote_utf9), u{x}         {}
    insn(uN x) : o(iop::quote_fn),   u{ζωp}, b(x) {}
    insn(St x) : o(iop::call_fn),    u{ζωp}, f(x) {}
  };

  struct block
  {
    frame   &f;
    V<insn>  is;  // uncompressed instructions

    block &operator<<(insn const &i) { is.push_back(i); return *this; }
  };


  M<St, uN> fn;  // name → fs[i]
  V<π0f>    fs;  // quotation and defined functions (not all functions are named)
  B         q;   // static quoted-value heap
  V<block>  bs;
  V<frame>  ss;


  frame &cs() { return ss[ss.size() - 1]; }
  block &cb() { return bs[bs.size() - 1]; }

  π0asm &def(Stc &n, π0f &&f)
    { A(!fn.contains(n), "π0asm redef " << n);
      fs.push_back(std::move(f));
      fn[n] = fs.size() - 1;
      return *this; }

  π0asm &lit(i9 x)
    {

    }
};


#if τdebug_iostream
O &operator<<(O &s, π0asm::iop const &o)
{
  switch (o)
  {
  case π0asm::iop::call_fn:    return s << "f()";
  case π0asm::iop::quote_fn:   return s << "'[]";
  case π0asm::iop::quote_utf9: return s << "'u9";
  }
}

O &operator<<(O &s, π0asm::insn const &i)
{
  switch (i.o)
  {
  case π0asm::iop::call_fn:    return s << i.f;
  case π0asm::iop::quote_fn:   return s << "[block=" << i.b << "]";
  case π0asm::iop::quote_utf9: return s << "'" << i.u;
  }
}

O &operator<<(O &s, π0asm::frame const &f)
{
  s << "| ";
  for (let &v : f.vs) s << v << " ";
  return s << "|";
}

O &operator<<(O &s, π0asm::block const &b)
{
  if (b.is.empty()) return s << "[]";
  s << "[" << b.is[0];
  for (uN i = 1; i < b.is.size(); ++i) s << " " << b.is[i];
  return s << "]";
}
#endif


}

#include "end.hh"


#endif
