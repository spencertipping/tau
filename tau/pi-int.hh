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


// π program interpreter
struct πi
{
  πi() {}
  πi(ξi i__, ξo o__) : i_(i__), o_(o__) {}

  η    y () const { return *i_; }
  η0o &ηo()       { return os_.back(); }

  template<η0ot T>
  πi &dpush(T  const &x) { is_.push_back(πv{x}); return *this; }
  πi &dpush(πv const &v) { is_.push_back(v);     return *this; }
  πv &dpeek() { return is_.back(); }
  πv  dpop()
    { A(!is_.empty(), "πi::dpop with empty is_");
      πv r = std::move(is_.back()); is_.pop_back(); return r; }


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


  Vc<πv>  &is() const { return is_; }
  Vc<η0o> &os() const { return os_; }
  Vc<uN>  &rs() const { return rs_; }


protected:
  ξi     i_;   // input channel
  ξo     o_;   // output channel
  V<πv>  is_;  // data stack
  V<η0o> os_;  // pending output values
  V<uN>  rs_;  // return stack
};


O &operator<<(O&, πi const&);


}

#include "end.hh"


#endif
