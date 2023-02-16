#ifndef τπint_h
#define τπint_h

#include "types.hh"
#include "eta.hh"
#include "xiio.hh"
#include "gamma.hh"
#include "pi-val.hh"

#include "begin.hh"

namespace τ
{


struct πinsn;
struct πfn;


// π program interpreter
struct πi
{
  πi(Sp<πfn const> f)             : f_(f) {}
  πi(Sp<πfn const> f, ξi i, ξo o) : f_(f), i_(i), o_(o) {}

  η    y () const { return i_.eof() ? nullptr : *i_; }
  η0o &ηo()       { return os_.back(); }

  template<η0ot T>
  πi &dpush(T  const &x) { is_.push_back(πv{x}); return *this; }
  πi &dpush(πv const &v) { is_.push_back(v);     return *this; }
  πv &dpeek() { return is_.back(); }
  πv  dpop()
    { A(!is_.empty(), "πi::dpop with empty is_");
      πv r = std::move(is_.back()); is_.pop_back(); return r; }

  πi &dclear() { is_.clear(); return *this; }


  πi &rpush(uN o)    { rs_.push_back(o); return *this; }
  uN &rpeek()        { return rs_.back(); }
  uN  rdepth() const { return rs_.size(); }
  uN  rpop()
    { A(!rs_.empty(), "πi::rpop with empty rs_");
      let r = rs_.back(); rs_.pop_back(); return r; }


  η0o &wbegin(η0t t) { os_.push_back(η0o{t}); return os_.back(); }
  πi  &wend()
    { A(!os_.empty(), "πi::wend with empty os_");
      dpush(std::move(os_.back())); os_.pop_back();
      return *this; }

  η0o &wv() { return os_.back(); }


  ξi &i() { return i_; }
  ξo &o() { return o_; }


  // Runs starting at the given bytecode, returning true on success
  // and false on error.
  //
  // The "each" function allows you to intercept each program step,
  // modify program state, and optionally exit the run loop (by
  // returning false).
  bool run(uN o = 0,
           F<bool(πi&, πinsn const&)> each
           = [](πi&, πinsn const&) { return true; });


  Vc<πv>  &is() const { return is_; }
  Vc<η0o> &os() const { return os_; }
  Vc<uN>  &rs() const { return rs_; }

  πfn const &f() const { return *f_; }


protected:
  Sp<πfn const> f_;
  ξi            i_;   // input channel
  ξo            o_;   // output channel
  V<πv>         is_;  // data stack
  V<η0o>        os_;  // pending output values
  V<uN>         rs_;  // return stack
};


O &operator<<(O&, πi const&);


}

#include "end.hh"


#endif
