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
  struct frame  // FIXME: rename
  {
    sc uNc fω = -1;
    V<St>  vs;

    frame(Stc &vs_)  // TODO: factor string-split into a utility function
      { St                x;
        std::stringstream s(vs_);
        while (s >> x) vs.push_back(x); }

    frame(Il<St> const &vs_)
      { std::copy(vs_.begin(), vs_.end(), std::back_inserter(vs)); }

    uN operator[](Stc &v)
      { for (uN i = 0; i < vs.size(); ++i) if (vs[i] == v) return i;
        return fω; }
  };

  struct block
  {
    V<uN> is;  // uncompressed instructions
    block &operator<<(uN i) { is.push_back(i); return *this; }
  };


  M<St, uN> fn;  // name → fs[i]
  M<uN, uN> gf;  // local getters (local index → fs[i])
  M<uN, uN> sf;  // local setters (local index → fs[i])
  V<π0f>    fs;  // quotation and defined functions (not all functions are named)
  B         qh;  // static quoted-value heap
  V<frame*> ss;
  V<block*> bs;


  frame &cs() { return *ss[ss.size() - 1]; }
  block &cb() { return *bs[bs.size() - 1]; }


  uN getf(Stc &n)
    { let i = cs()[n];
      if (!gf.contains(i)) gf[i] = def([i](π0int &z) { z.h.dpush(z.h.fi(i)); });
      return gf[i]; }

  uN setf(Stc &n)
    { let i = cs()[n.back() == '=' ? n.substr(0, n.size() - 1) : n];
      if (!sf.contains(i)) sf[i] = def([i](π0int &z) { z.h.fs(i, z.h.di(0)); z.h.dpop(); });
      return sf[i]; }

  template<class T>
  π0asm &def(Stc &n, T &&f)
    { A(!fn.contains(n), "π0asm redef " << n);
      fn[n] = def(std::move(f));
      return *this; }

  uN                   def(π0f &&f)                 { fs.push_back(std::move(f)); return fs.size() - 1; }

  uN                   def(F<void(i9)>         &&f) { return def([f = std::move(f)](π0int &z) { f(z.h.di(0));                       z.h.dpop();  }); }
  uN                   def(F<void(i9, i9)>     &&f) { return def([f = std::move(f)](π0int &z) { f(z.h.di(0), z.h.di(1));            z.h.dpop(2); }); }
  uN                   def(F<void(i9, i9, i9)> &&f) { return def([f = std::move(f)](π0int &z) { f(z.h.di(0), z.h.di(1), z.h.di(2)); z.h.dpop(3); }); }

  template<class T> uN def(F<T(i9)>            &&f) { return def([f = std::move(f)](π0int &z) { let r = f(z.h.di(0));                       z.h.dpop();  z.h.dpush(r); }); }
  template<class T> uN def(F<T(i9, i9)>        &&f) { return def([f = std::move(f)](π0int &z) { let r = f(z.h.di(0), z.h.di(1));            z.h.dpop(2); z.h.dpush(r); }); }
  template<class T> uN def(F<T(i9, i9, i9)>    &&f) { return def([f = std::move(f)](π0int &z) { let r = f(z.h.di(0), z.h.di(1), z.h.di(2)); z.h.dpop(3); z.h.dpush(r); }); }


  template<class T>
  π0asm &lit(T const &x)
    { let i = qh << o9(x);
      cb() << def([i](π0int &z) { z.h.dpush(o9(z[i])); });
      return *this; }


  // Block authoring functions
  π0asm &block() { bs.push_back(new struct block()); return *this; }
  π0asm &frame(Stc &vs) { TODO("π0asm frame"); }


  π0asm &q_(Stc &c)
    { if (fn.contains(c)) { cb() << fn.at(c); return *this; }
      if (c.back() == '=') { cb() << setf(c); return *this; }
      cb() << getf(c);
      return *this; }

  // TODO: more inline literals?
  π0asm &q_(uN x) { return lit(x); }
  π0asm &q_(iN x) { return lit(x); }

  template<o9__ T> π0asm &q_(T const &x) { return lit(x); }


  template<class X, class... Xs>
  π0asm &q(X const &x, Xs... xs) { q_(x); return q(xs...); }
  π0asm &q()                     { return *this; }
};


#if τdebug_iostream
O &operator<<(O &s, struct π0asm::frame const &f)
{
  s << "| ";
  for (let &v : f.vs) s << v << " ";
  return s << "|";
}

O &operator<<(O &s, struct π0asm::block const &b)
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
