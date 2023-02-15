#ifndef τπfn_h
#define τπfn_h

#include <algorithm>

#include "pi-int.hh"
#include "begin.hh"

namespace τ
{


typedef bool πinsn_ret;

sletc πinsn_ok    = true;
sletc πinsn_error = false;


// Single instruction within a program
struct πinsn
{
  St                name;
  F<πinsn_ret(πi&)> f;
};


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
      r << πinsn{"[", [n=fs.size()](πi &i) { i.rpeek() += n + 1; return πinsn_ok; }};
      r += *this;
      r << πinsn{"]", []           (πi &i) { i.rpop(); return πinsn_ok; }};
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
