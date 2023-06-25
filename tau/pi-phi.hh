#ifndef τπφ_h
#define τπφ_h

#include "types.hh"
#include "phi.hh"
#include "phi-ctor.hh"
#include "phi-auto.hh"
#include "phi-markers.hh"
#include "pi-auto.hh"
#include "pi-fn.hh"
#include "pi-int.hh"
#include "pi-phi-basic.hh"
#include "pi-phi-markers.hh"
#include "tau-phi.hh"

#include "begin.hh"

namespace τ
{


// An extensible π grammar. See pi-phi.md for details.
//
// To use this struct, inherit from it for the core grammar and mix in
// structs from pi-phi-basic.hh to add parsers.
template<class πφ> struct πφ_
{
  πφ_();
  virtual ~πφ_() {}

  πφ       &self()       { return *Rc<πφ*>(this); }
  πφ const &self() const { return *Rc<πφ const*>(this); }


  // Helpers for prefix and postfix operators
  static π1 pre (π0 a, π1 b)      { return b | a; }
  static π1 post(π1 a, Vc<π0> &b) { π1 r = a; r << b; return r; }


  φ<π1> t()   const { return wt_; }

  φ<π1> ts()  const { return wse_; }
  φ<π1> tp()  const { return wpe_; }
  φ<π0> tsp() const { return wsp_; }
  φ<π0> tpp() const { return wpp_; }


  πφ &def_t (φ<π1> const &p) { t_ .as<φa_<π1>>() << p; return self(); }
  πφ &def_sa(φ<π1> const &p) { s_ .as<φa_<π1>>() << p; return self(); }
  πφ &def_pa(φ<π1> const &p) { p_ .as<φa_<π1>>() << p; return self(); }
  πφ &def_st(φ<π0> const &p) { st_.as<φa_<π0>>() << p; return self(); }
  πφ &def_pt(φ<π0> const &p) { pt_.as<φa_<π0>>() << p; return self(); }

  Tt If<!Eq<T, φ<π1>>, πφ> &def_t (T const &f) { return def_t (φauto(self(), f)); }
  Tt If<!Eq<T, φ<π1>>, πφ> &def_sa(T const &f) { return def_sa(φauto(self(), f)); }
  Tt If<!Eq<T, φ<π1>>, πφ> &def_pa(T const &f) { return def_pa(φauto(self(), f)); }
  Tt If<!Eq<T, φ<π0>>, πφ> &def_st(T const &f) { return def_st(φauto(self(), f)); }
  Tt If<!Eq<T, φ<π0>>, πφ> &def_pt(T const &f) { return def_pt(φauto(self(), f)); }

  Tt πφ &def_ssp(St n, T const &f) { return def_(ssp_, n, f); }
  Tt πφ &def_psp(St n, T const &f) { return def_(psp_, n, f); }
  Tt πφ &def_spp(St n, T const &f) { return def_(spp_, n, f); }
  Tt πφ &def_ppp(St n, T const &f) { return def_(ppp_, n, f); }
  Tt πφ &def_sp (St n, T const &f) { return def_(sp_,  n, f); }
  Tt πφ &def_pp (St n, T const &f) { return def_(pp_,  n, f); }


  // NOTE: returning the wrong type is intentional. Every πsa<T> should
  // arise from a π1 that we parse, and it's up to πauto to convert the
  // π1 to T using immediate stack indirection. See pi-auto for details.
  Tt φ<πt <π1>> p(πt <T>*) const { return wt_  * [](π1 &&x) { return πt <π1>{mo(x)}; }; }
  Tt φ<πsa<π1>> p(πsa<T>*) const { return ws_  * [](π1 &&x) { return πsa<π1>{mo(x)}; }; }
  Tt φ<πpa<π1>> p(πpa<T>*) const { return wp_  * [](π1 &&x) { return πpa<π1>{mo(x)}; }; }
  Tt φ<πse<π1>> p(πse<T>*) const { return wse_ * [](π1 &&x) { return πse<π1>{mo(x)}; }; }
  Tt φ<πpe<π1>> p(πpe<T>*) const { return wpe_ * [](π1 &&x) { return πpe<π1>{mo(x)}; }; }

  // These are typed as expected because they must be treated as lazy
  // functions, which is consistent with the type signature.
  φ<πst<π0>> p(πst<π0>*) const { return wst_ * [](π0 &&x) { return πst<π0>{mo(x)}; }; }
  φ<πpt<π0>> p(πpt<π0>*) const { return wpt_ * [](π0 &&x) { return πpt<π0>{mo(x)}; }; }


protected:
  Tt πφ &def_(φ<π0> &d, Stc &n, T const &f)
    { d.as<φd_<π0>>().def(n, πauto(self(), n, std::function(f)));
      return self(); }

  φ<π1> t_;    // toplevel program (alt)

  φ<π1> se_;   // singular expressions (alt)
  φ<π1> pe_;   // plural expressions (alt)

  φ<π1> s_;    // singular atoms (alt)
  φ<π1> p_;    // plural atoms (alt)

  φ<π0> ssp_;  // singular-operand, singular-return prefix (dispatch)
  φ<π0> psp_;  // plural-operand, singular-return prefix (dispatch)
  φ<π0> spp_;  // singular-operand, plural-return prefix (dispatch)
  φ<π0> ppp_;  // plural-operand, plural-return prefix (dispatch)
  φ<π0> sp_;   // singular → singular postfix (dispatch)
  φ<π0> pp_;   // plural → plural postfix (dispatch)

  φ<π0> st_;   // singular-returning transformer (alt)
  φ<π0> pt_;   // plural-returning transformer (alt)

  // Wrapped+weakened versions of the above, which allow whitespace
  φ<π1> wt_;   // wrap(φW(t_))

  φ<π1> wse_;
  φ<π1> wpe_;
  φ<π1> ws_;
  φ<π1> wp_;

  φ<π0> wssp_;
  φ<π0> wpsp_;
  φ<π0> wspp_;
  φ<π0> wppp_;
  φ<π0> wsp_;
  φ<π0> wpp_;

  φ<π0> wst_;
  φ<π0> wpt_;
};


template<class πφ>
πφ_<πφ>::πφ_()
  : t_  (φa0<π1>("π")),
    se_ (φa0<π1>("πse")),
    pe_ (φa0<π1>("πpe")),
    s_  (φa0<π1>("πs")),
    p_  (φa0<π1>("πp")),
    ssp_(φd <π0>("πssp")),
    psp_(φd <π0>("πpsp")),
    spp_(φd <π0>("πspp")),
    ppp_(φd <π0>("πppp")),
    sp_ (φd <π0>("πsp")),
    pp_ (φd <π0>("πpp")),
    st_ (φa0<π0>("πst")),
    pt_ (φa0<π0>("πpt")),

    wt_  (φwrap(φW(t_))),
    wse_ (φwrap(φW(se_))),
    wpe_ (φwrap(φW(pe_))),
    ws_  (φwrap(φW(s_))),
    wp_  (φwrap(φW(p_))),
    wssp_(φwrap(φW(ssp_))),
    wpsp_(φwrap(φW(psp_))),
    wspp_(φwrap(φW(spp_))),
    wppp_(φwrap(φW(ppp_))),
    wsp_ (φwrap(φW(sp_))),
    wpp_ (φwrap(φW(pp_))),
    wst_ (φwrap(φW(st_))),
    wpt_ (φwrap(φW(pt_)))
{
  let s1 = ws_ | (wssp_ & φW(wse_)) % pre;  // s_atom | ss_pre s
  let s2 =       (wpsp_ & φW(wpe_)) % pre;  // ps_pre p
  let s3 = (s1 & φn(wsp_)) % post;          // (s_atom | ss_pre s) s_post*
  se_.as<φa_<π1>>() << s3 << s2;

  let p1 = wp_
    | (wspp_ & φW(wse_)) % pre
    | (wppp_ & φW(wpe_)) % pre;
  let p2 = (p1 & φn(wpp_)) % post;
  pe_.as<φa_<π1>>() << p1 << p2 << wse_;

  def_sa(φgroup(wse_));
  def_pa(φgroup(πφnp(wpe_)));

  // TODO '(' ')' groups

  def_st(wsp_);  // inline transform
  def_pt(wpp_);  // inline transform

  def_t((φn(φ1("πpe`", wpe_, φl("`"))) & wpe_)
        % [](Vc<π1> &xs, π1 &y)
          { π0 r;
            for (let &x : xs) r << (x | πf<-1>{"_", [](πi &i) { i.pop(); }});
            return r | y; });
}


template<class... Xs>
struct πφ : public πφ_<πφ<Xs...>>, φauto_str, Xs...
{
  using πφ_<πφ<Xs...>>::p;
  using φauto_str::p;
  using Xs::p...;
};


}

#include "end.hh"

#endif
