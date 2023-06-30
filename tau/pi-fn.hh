#ifndef τπfn_h
#define τπfn_h

#include "pi-int.hh"
#include "begin.hh"

namespace τ
{


// A π function that can act on an interpreter, and which adjusts the
// stack size by N.
template<iN N>
struct πf final
{
  πf() = default;
  πf(St s, F<void(πi&)> const &f) { ss.push_back(s); fs.push_back(f); }

  πi &operator()(πi &i) const { for (let &f : fs) f(i); return i; }

  template<iN M>
  πf<N + M> operator|(πf<M> const &f) const
    { πf<N + M> r;
      std::copy(  ss.begin(),   ss.end(), std::back_inserter(r.ss));
      std::copy(f.ss.begin(), f.ss.end(), std::back_inserter(r.ss));
      std::copy(  fs.begin(),   fs.end(), std::back_inserter(r.fs));
      std::copy(f.fs.begin(), f.fs.end(), std::back_inserter(r.fs));
      return r; }

  πf<N> &operator<<(πf<0> const &f)
    { std::copy(f.ss.begin(), f.ss.end(), std::back_inserter(ss));
      std::copy(f.fs.begin(), f.fs.end(), std::back_inserter(fs));
      return *this; }

  πf<N> &operator<<(V<πf<0>> const &f)
    { for (let &x : f) *this << x; return *this; }

  V<St>           ss;
  V<F<void(πi&)>> fs;
};


typedef πf<0>  π0;
typedef πf<1>  π1;
typedef πf<-1> π_1;


inline π_1 πf_pop() { return {"_", [](πi &i) { i.pop(); }}; }
inline π0  πf_η()   { return {"η", [](πi &i) { i.push(i.ypop()); }}; }


template<iN N>
O &operator<<(O &s, πf<N> const &f)
{
  s << "[";
  bool first = true;
  for (let &x : f.ss)
  { if (first) first = false;
    else       s << " ";
    s << x; }
  return s << "]";
}


}

#include "end.hh"

#endif
