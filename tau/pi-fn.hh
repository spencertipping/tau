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

  void operator()(πi &i) const { for (let &f : fs) f(i); }

  template<iN M>
  πf<N + M> operator|(πf<M> const &f) const
    { πf<N + M> r;
      std::copy(  ss.begin(),   ss.end(), std::back_inserter(r.ss));
      std::copy(f.ss.begin(), f.ss.end(), std::back_inserter(r.ss));
      std::copy(  fs.begin(),   fs.end(), std::back_inserter(r.fs));
      std::copy(f.fs.begin(), f.fs.end(), std::back_inserter(r.fs));
      return r; }

  V<St>           ss;
  V<F<void(πi&)>> fs;
};


template<iN N>
O &operator<<(O &s, πf<N> &f)
{
  s << "πf<" << N << ">[";
  bool first = true;
  for (let &x : f.ss)
  { s << x;
    if (first) first = false;
    else       s << " "; }
  return s << "]";
}


}

#include "end.hh"

#endif
