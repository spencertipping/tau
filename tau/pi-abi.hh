#ifndef τπabi_h
#define τπabi_h


#include "../dep/picosha2.h"


#include "types.hh"
#include "pi-types.hh"

#include "begin.hh"

namespace τ
{


struct πabi
{
  V<πf>       f;       // function table (π0fi to index)
  V<St>       n;       // function names, to list disassembly
  S<St>       ns;      // set of defined names
  πav mutable v_ = 0;  // cached version

  πav v() const
  { if (v_) return v_;
    Stc nul = "\0";
    picosha2::hash256_one_by_one h;
    for (let &s : n)
    { h.process(s  .begin(), s  .end());
      h.process(nul.begin(), nul.end()); }
    h.finish();
    V<u8> r(picosha2::k_digest_size);
    h.get_hash_bytes(r.begin(), r.end());
    return v_ = R<u64>(r.data(), 0); }

  πabi &def(Stc &n_, πf f_)
  { A(!v_, "defining " << n_ << " on versioned π₀abi");
    A(ns.emplace(n_).second, "redefining " << n_);
    f.push_back(f_);
    n.push_back(n_);
    return *this; }

  uN size() const { return f.size(); }
};


#if τdebug_iostream
O &operator<<(O &s, πabi const &a)
{
  TODO("<<π0abi");
}
#endif


}

#include "end.hh"


#endif
