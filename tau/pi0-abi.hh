#ifndef τπ0abi_h
#define τπ0abi_h


#include "../dep/picosha2.h"


#include "types.hh"
#include "pi0-types.hh"

#include "pi0-begin.hh"

namespace τ
{


π0TGs π0abi
{
  π0TS;
  V<π0T(π0f)>  f;       // function table (π0fi to index)
  V<St>        n;       // function names, to list disassembly
  S<St>        ns;      // set of defined names
  π0av mutable v_ = 0;  // cached version

  π0av v() const
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

  π0abi &def(Stc &n_, π0T(π0f) f_)
  { A(!v_, "defining " << n_ << " on versioned π₀abi");
    A(ns.emplace(n_).second, "redefining " << n_);
    f.push_back(f_);
    n.push_back(n_);
    return *this; }
};


#if τdebug_iostream
π0TG O &operator<<(O &s, π0T(π0abi) const &a)
{
  TODO("<<π0abi");
}
#endif


}

#include "pi0-end.hh"


#endif
