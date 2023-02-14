#ifndef τπint_h
#define τπint_h

#include "types.hh"
#include "eta.hh"
#include "xiio.hh"
#include "gamma.hh"

#include "begin.hh"

namespace τ
{


struct πi;
struct πv;

template<class T> using πf = F<T(πi&)>;


// Evaluation function: escape by returning true
typedef πf<bool> πef;


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


// π program interpreter
struct πi
{
  πi() {}
  πi(ξi i__, ξo o__) : i_(i__), o_(o__) {}

  η    y () const { return *i_; }
  η0o &ηo()       { return os_.back(); }


protected:
  ξi     i_;
  ξo     o_;
  V<πv>  is_;
  V<η0o> os_;
};


}

#include "end.hh"


#endif
