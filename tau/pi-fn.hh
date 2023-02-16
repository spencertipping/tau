#ifndef τπfn_h
#define τπfn_h

#include <algorithm>

#include "pi-int.hh"
#include "strings.hh"
#include "begin.hh"

namespace τ
{


typedef bool πinsn_ret;

sletc πinsn_ok    = true;
sletc πinsn_error = false;


// Single instruction within a program
struct πinsn
{
  πinsn(St name_, F<πinsn_ret(πi&)> f_) : name(name_), f(f_) {}
  πinsn(St name_, F<void(πi&)> f_)
    : name(name_), f([f_](πi &i) { f_(i); return πinsn_ok; }) {}

  St                name;
  F<πinsn_ret(πi&)> f;
};


template<class T>
std::enable_if_t<!std::is_same_v<T, γ>, πinsn>
πpush(T const &x)
{ return πinsn((Ss{} << "push " << x).str(), [x](πi &i) { i.dpush(η1o(x)); }); }

inline πinsn πpush(γ x)
{ return πinsn((Ss{} << "push " << x).str(), [x](πi &i) { i.dpush(x); }); }


inline πinsn πf(St n, F<γ(γ)> f) { return πinsn(n, [f](πi &i) { i.dpush(f(i.dpop().as_γ())); }); }
inline πinsn πf(St n, F<γ(η)> f) { return πinsn(n, [f](πi &i) { i.dpush(f(i.dpop().as_η())); }); }

inline πinsn πf(St n, F<γ(i64)>   f) { return πinsn(n, [f](πi &i) { i.dpush(f(i.dpop().as_η().pi())); }); }
inline πinsn πf(St n, F<γ(η1sig)> f) { return πinsn(n, [f](πi &i) { i.dpush(f(i.dpop().as_η().ps())); }); }
inline πinsn πf(St n, F<γ(St)>    f) { return πinsn(n, [f](πi &i) { i.dpush(f(i.dpop().as_η().st())); }); }
inline πinsn πf(St n, F<γ(cs7)>   f) { return πinsn(n, [f](πi &i) { i.dpush(f(i.dpop().as_η().st().c_str())); }); }


// π program/function (which are the same thing)
struct πfn
{
  πfn() {}
  πfn(πinsn const &f) { fs.push_back(f); }

  πfn operator+(πfn const &x) const
    { πfn r;
      r += *this;
      r += x;
      return r; }

  πfn &operator+=(πfn   const &x) { std::copy(x.fs.begin(), x.fs.end(), std::back_inserter(fs)); return *this; }
  πfn &operator<<(πinsn const &i) { fs.push_back(i); return *this; }

  πfn q() const
    { πfn r;
      r << πinsn{"[", [n=fs.size()](πi &i)
        { i.dpush(η1o(i.rpeek()));
          i.rpeek() += n + 1; }};
      r += *this;
      r << πinsn{"]", [](πi &i) { i.rpop(); }};
      return r; }

  // Runs this function on the given interpreter, returning true on success
  // and false on error.
  //
  // The "each" function allows you to intercept each program step, modify
  // program state, and optionally exit the run loop (by returning false).
  bool run(πi &i, F<bool(πi&, πinsn const&)> each = [](πi&, πinsn const&) { return true; }) const;

  V<πinsn> fs;
};


O &operator<<(O&, πinsn const&);
O &operator<<(O&, πfn   const&);


}

#include "end.hh"

#endif
