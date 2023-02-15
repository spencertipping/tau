#ifndef τπval_h
#define τπval_h


#include "gamma.hh"
#include "eta.hh"

#include "begin.hh"

namespace τ
{


// Dynamically-typed π value
struct πv
{
  template<η0ot T>
  πv(T const &x)
    { B r; r.resize(x.osize());
      x.into(r.data());
      v_ = std::move(r); }

  πv(γ g) { v_ = g; }

  bool is_η() const { return v_.index() == 0; }
  bool is_γ() const { return v_.index() == 1; }

  η as_η() const { return η(std::get<0>(v_).data()); }
  γ as_γ() const { return   std::get<1>(v_); }

protected:
  Va<B, γ> v_;
};


O &operator<<(O&, πv const&);


}

#include "end.hh"


#endif
