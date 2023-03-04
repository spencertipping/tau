#ifndef τπval_h
#define τπval_h


#include "Gamma.hh"
#include "eta.hh"

#include "begin.hh"

namespace τ
{


// Dynamically-typed π value
// TODO: expand the variant to include shared_ptrs to C++ things
//
// (NOTE: no point in doing C++ primitives because basic_string
//  inlining is sufficiently fast)

struct πv
{
  template<η0ot T>
  πv(T const &x)
    { B r; r.resize(x.osize());
      x.into(r.data());
      v_ = std::move(r); }

  πv(Γ g) { v_ = g; }

  bool is_η() const { return v_.index() == 0; }
  bool is_Γ() const { return v_.index() == 1; }

  η as_η() const { A(is_η(), "as_η on Γ"); return η(std::get<0>(v_).data()); }
  Γ as_Γ() const { A(is_Γ(), "as_Γ on η"); return   std::get<1>(v_); }

protected:
  Va<B, Γ> v_;
};


O &operator<<(O&, πv const&);


}

#include "end.hh"


#endif
