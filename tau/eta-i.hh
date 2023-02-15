#ifndef τηi_h
#define τηi_h


#include "eta-types.hh"
#include "eta0i.hh"
#include "eta1i.hh"
#include "eta0o.hh"
#include "eta1o.hh"

#include "begin.hh"

namespace τ
{


// Multi-level η reader with builtin type casting
struct η
{
  η(u8c *a)       : i_(a) {}
  η(η0i const &i) : i_(i) {}


  // Explicitly destroy any resources held by this or any structural descendant
  void destroy()
    { if      (f() && η1tc[t()]) for (auto x : T()) x.destroy();
      else if (ty())
        if (let f = pxf<void(*)(void*)>()) (*f)(pv<void*>()); }


  η &operator=(u8c       *a) { i_ = a;    return *this; }
  η &operator=(η0i const &i) { i_ = i;    return *this; }
  η &operator=(η   const &y) { i_ = y.i_; return *this; }

  bool exists() const { return i_.exists(); }


  η0t  t() const { return i_.type(); }
  u8   c() const { return i_.c(); }
  bool f() const { return i_.f(); }
  bool h() const { return i_.h(); }
  bool v() const { return i_.v(); }


  bool ty()  const { return t() == η0t::η0; }
  bool tf()  const { return t() == η0t::float_be; }
  bool tu()  const { return t() == η0t::uint_be; }
  bool ti()  const { return t() == η0t::int_be; }
  bool tb()  const { return t() == η0t::boolean; }
  bool ts()  const { return t() == η0t::signal; }
  bool tsb() const { return t() == η0t::bytes; }
  bool tsu() const { return t() == η0t::utf8; }

  bool tT()  const { return t() == η0t::tuple; }
  bool tS()  const { return t() == η0t::set; }
  bool tM()  const { return t() == η0t::map; }


  f64   pf() const { A(tf(), "η::pf on " << i_); return Sc<f64>(η1pi{&i_}); }
  u64   pu() const { A(tu(), "η::pu on " << i_); return Sc<u64>(η1pi{&i_}); }
  i64   pi() const { A(ti(), "η::pi on " << i_); return Sc<i64>(η1pi{&i_}); }
  bool  pb() const { A(tb(), "η::pb on " << i_); return Sc<bool>(η1pi{&i_}); }
  η1sig ps() const { A(ts(), "η::ps on " << i_); return Sc<η1sig>(η1si{&i_}); }


  // pv = value pointer accessor; pxf = value destructor function accessor
  template<class T> T *pv()  const { A(ty(), "η::pv on "  << i_); return Rc<T*>(R<void*>(i_.data(), 0)); }
  template<class T> T *pxf() const { A(ty(), "η::pxf on " << i_); return Rc<T*>(R<void*>(i_.data(), sizeof(void*))); }


  bool α() const { return ts() && ps() == η1sig::α; }
  bool κ() const { return ts() && ps() == η1sig::κ; }
  bool ι() const { return ts() && ps() == η1sig::ι; }
  bool τ() const { return ts() && ps() == η1sig::τ; }
  bool ω() const { return ts() && ps() == η1sig::ω; }


  Stvc stv() const { return Stvc{Rc<chc*>(data()), size()}; }
  St   st()  const { return St  {Rc<chc*>(data()), size()}; }
  Bvc  bv()  const { return Bvc {data(),           size()}; }
  B    b ()  const { return B   {data(),           size()}; }


  // Inner (logical, decompressed) data and size; use these when reading
  // contents
  u8c *data() const { return i_.data(); }
  uN   size() const { return i_.size(); }


  // Logical number of items for strings/collections
  uN len() const
    { switch (t())
      {
      case η0t::bytes:
      case η0t::utf8: return size();  // TODO: decode UTF8 chars
      case η0t::tuple:
      case η0t::set: return η1ti{&i_}.len();
      case η0t::map: return η1ti{&i_}.len() / 2;
      default:
        A(0, "η::len on " << i_);
        return 0;
      } }


  // Structured retrieval
  η operator[](uN i) const
    { A(t() == η0t::tuple, "η[] on " << i_);
      return η1ti{&i_}[i]; }

  // TODO: map element retrieval


  // Tuple/set iterator
  struct itT
  {
    u8c *x;
    itT &operator++()                   { x += η0i{x}.osize(); return *this; }
    η    operator* ()             const { return η{x}; }
    bool operator==(itT const &y) const { return x == y.x; }
  };

  struct T_
  {
    η const &y;
    itT begin() { return itT{y.data()}; }
    itT end()   { return itT{y.data() + y.size()}; }
  };


  // Map iterator: bind pairs of values
  struct itM
  {
    u8c *x;
    itM    &operator++()                   { x += η0i{x}.osize(); x += η0i{x}.osize(); return *this; }
    P<η, η> operator* ()             const { return mp(η{x}, η{x + η0i{x}.osize()}); }
    bool    operator==(itT const &y) const { return x == y.x; }
  };

  struct M_
  {
    η const &y;
    itM begin() { return itM{y.data()}; }
    itM end()   { return itM{y.data() + y.size()}; }
  };


  T_ T() const { return T_{*this}; }
  T_ S() const { return T_{*this}; }
  M_ M() const { return M_{*this}; }


  // TODO: equivalence
  // TODO: ordering
  // TODO: hashing


  η1i        y1() const { return η1i{&i_}; }
  η0i const &y0() const { return i_; }
  η0i       &y0()       { return i_; }


  u8c *odata()      const { return y0().odata(); }
  uN   osize()      const { return y0().osize(); }
  void into (u8 *m) const { return y0().into(m); }


protected:
  η0i i_;
};


template<> struct η0ot_<η> { sletc v = true; };


inline η0o &operator<<(η0o &c, η const &y)
{
  A(η1tc[c.t()], "η₀o<<η with " << c.t());
  memcpy(c.iptr(y.y0().osize()), y.y0().odata(), y.y0().osize());
  return y.f() ? c.f(true) : c;
}


inline η0o η1o(η const &x) { return η1o(x.y0()); }


O &operator<<(O&, η const&);


}

#include "end.hh"

#endif
