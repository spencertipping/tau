#ifndef σpre_container_h
#define σpre_container_h

#include "../tau.hh"
#include "../tau/begin.hh"

namespace σ::pre
{

using namespace τ;


struct ct_fifo     {};
struct ct_prio_min {};
struct ct_prio_max {};
struct ct_bloom    { u64 n; f64 fp; };
struct ct_uniq     {};
struct ct_set      {};
struct ct_map      {};
struct ct_multimap {};

typedef Va<ct_fifo, ct_prio_min, ct_prio_max,  // 0..2
           ct_bloom, ct_uniq,                  // 3..4
           ct_set, ct_map, ct_multimap> ctype; // 5..7


struct cb_native   {};
struct cb_disk_tmp { bytes n; };
struct cb_sqlite   { St db; St t; };

typedef Va<cb_native, cb_disk_tmp, cb_sqlite> cback;


template<class A> struct at_parsers
{
  auto p(ct_fifo*)     const { return φl("=") * [](auto) { return ct_fifo{}; }; }
  auto p(ct_prio_min*) const { return φl("<") * [](auto) { return ct_prio_min{}; }; }
  auto p(ct_prio_max*) const { return φl(">") * [](auto) { return ct_prio_max{}; }; }
  auto p(ct_uniq*)     const { return φl("u") * [](auto) { return ct_uniq{}; }; }
  auto p(ct_set*)      const { return φl("?") * [](auto) { return ct_set{}; }; }
  auto p(ct_map*)      const { return φl(":") * [](auto) { return ct_map{}; }; }
  auto p(ct_multimap*) const { return φl(";") * [](auto) { return ct_multimap{}; }; }

  auto p(ct_bloom*) const
    { return φauto(*Rc<A const*>(this), [](φaL<'B'>, u64 n, f64 fp) { return ct_bloom{n, fp}; }); }

  auto p(cb_native*)   const { return φR(cb_native{}); }
  auto p(cb_disk_tmp*) const { return φauto(*Rc<A const*>(this), [](bytes b) { return cb_disk_tmp{b}; }); }
  auto p(cb_sqlite*)   const { return φauto(*Rc<A const*>(this), [](St s)
    { let i = s.find(':');
      A(i != s.npos, "cb_sqlite → " << s << " unsupported");
      return cb_sqlite{s.substr(0, i), s.substr(i + 1)}; }); }
};


struct at_;

O &operator<<(O&, ctype const&);
O &operator<<(O&, cback const&);
O &operator<<(O&, at_   const&);


struct at_
{
  at_(ctype const &t, cback const &b) : t_(t), b_(b) {}
  virtual ~at_() {}

  virtual void α(ηic&, ξo) { A(0, "α → " << *this << " unsupported"); }
  virtual void ω(ηic&, ξo) { A(0, "ω → " << *this << " unsupported"); }
  virtual void κ(ηic&, ξo) { A(0, "κ → " << *this << " unsupported"); }
  virtual void ι(ηic&, ξo) { A(0, "ι → " << *this << " unsupported"); }
  virtual void ρ(ηic&, ξo) { A(0, "ρ → " << *this << " unsupported"); }
  virtual void τ(ηic&, ξo) { A(0, "τ → " << *this << " unsupported"); }

  ctype t_;
  cback b_;
};


Sp<at_> at(ctype, cback);


}

#include "../tau/end.hh"

#endif
