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


struct π0afr  // π₀ asm frame
{
  sc uNc fω = -1;
  V<St>  vs;

  π0afr(Stc &vs_)  // TODO: factor string-split into a utility function
    { St                x;
      std::stringstream s(vs_);
      while (s >> x) vs.push_back(x); }

  π0afr(Il<St> const &vs_)
    { std::copy(vs_.begin(), vs_.end(), std::back_inserter(vs)); }

  uN operator[](Stc &v)
    { for (uN i = 0; i < vs.size(); ++i) if (vs[i] == v) return i;
      return fω; }
};


struct π0abl  // π₀ asm block
{
  V<uN> is;  // uncompressed instructions
  π0abl &operator<<(uN i) { is.push_back(i); return *this; }
};


struct π0asm
{
  M<St, uN> fn;   // name → fs[i]
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
      cb() << def(
        [n = b->is.size(), r](π0int &i)
        { i.h.dpush(i.r.back());
          i.r.back() += n + r; });
      for (let i : b->is) cb() << i;
      if (r) cb() << ret;
      delete b;
      return *this; }

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


  π0int build()
    { A(bls.size() == 1, "π₀ bs=" << bls.size() << " ≠ 1");
      cb() << ret;
      return π0int(std::move(qh), std::move(fs),
                   std::move(cb().is), 0); }
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

#include "end.hh"


#endif
