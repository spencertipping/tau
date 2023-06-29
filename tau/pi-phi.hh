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
  πφ &def_sa(φ<π1> const &p) { sa_.as<φa_<π1>>() << p; return self(); }
  πφ &def_pa(φ<π1> const &p) { pa_.as<φa_<π1>>() << p; return self(); }

  Tt If<!Eq<T, φ<π1>>, πφ> &def_t (T const &f) { return def_t (φauto(self(), f)); }
  Tt If<!Eq<T, φ<π1>>, πφ> &def_sa(T const &f) { return def_sa(φauto(self(), f)); }
  Tt If<!Eq<T, φ<π1>>, πφ> &def_pa(T const &f) { return def_pa(φauto(self(), f)); }

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
  Tt φ<πsa<π1>> p(πsa<T>*) const { return wsa_ * [](π1 &&x) { return πsa<π1>{mo(x)}; }; }
  Tt φ<πpa<π1>> p(πpa<T>*) const { return wpa_ * [](π1 &&x) { return πpa<π1>{mo(x)}; }; }
  Tt φ<πse<π1>> p(πse<T>*) const { return wse_ * [](π1 &&x) { return πse<π1>{mo(x)}; }; }
  Tt φ<πpe<π1>> p(πpe<T>*) const { return wpe_ * [](π1 &&x) { return πpe<π1>{mo(x)}; }; }


protected:
  Tt πφ &def_(φ<π0> &d, Stc &n, T const &f)
    { d.as<φd_<π0>>().def(n, πauto(self(), n, std::function(f)));
      return self(); }

  φ<π1> t_;    // toplevel program (alt)

  φ<π1> se_;   // singular expressions (alt)
  φ<π1> pe_;   // plural expressions (alt)

  φ<π1> sa_;    // singular atoms (alt)
  φ<π1> pa_;    // plural atoms (alt)

  φ<π0> ssp_;  // singular-operand, singular-return prefix (dispatch)
  φ<π0> psp_;  // plural-operand, singular-return prefix (dispatch)
  φ<π0> spp_;  // singular-operand, plural-return prefix (dispatch)
  φ<π0> ppp_;  // plural-operand, plural-return prefix (dispatch)
  φ<π0> sp_;   // singular → singular postfix (dispatch)
  φ<π0> pp_;   // plural → plural postfix (dispatch)

  // Wrapped+weakened versions of the above, which allow whitespace
  φ<π1> wt_;   // wrap(φW(t_))

  φ<π1> wse_;
  φ<π1> wpe_;  // πφnp(wrap(φW(pe_)) -- this one is different
  φ<π1> wsa_;
  φ<π1> wpa_;

  φ<π0> wssp_;
  φ<π0> wpsp_;
  φ<π0> wspp_;
  φ<π0> wppp_;
  φ<π0> wsp_;
  φ<π0> wpp_;
};


template<class πφ>
πφ_<πφ>::πφ_()
  : t_  (φa0<π1>("π")),
    se_ (φa0<π1>("πse")),
    pe_ (φa0<π1>("πpe")),
    sa_ (φa0<π1>("πsa")),
    pa_ (φa0<π1>("πpa")),
    ssp_(φd <π0>("πssp")),
    psp_(φd <π0>("πpsp")),
    spp_(φd <π0>("πspp")),
    ppp_(φd <π0>("πppp")),
    sp_ (φd <π0>("πsp")),
    pp_ (φd <π0>("πpp")),

    wt_  (φwrap(φW(t_))),
    wse_ (φwrap(φW(se_))),
    wpe_ (πφnp(φwrap(φW(pe_)))),
    wsa_ (φwrap(φW(sa_))),
    wpa_ (φwrap(φW(pa_))),
    wssp_(φwrap(φW(ssp_))),
    wpsp_(φwrap(φW(psp_))),
    wspp_(φwrap(φW(spp_))),
    wppp_(φwrap(φW(ppp_))),
    wsp_ (φwrap(φW(sp_))),
    wpp_ (φwrap(φW(pp_)))
{
  let s1 = wsa_ | (wssp_ & wse_) % pre;  // s_atom | ss_pre s
  let s2 =        (wpsp_ & wpe_) % pre;  // ps_pre p
  let s3 = (s1 & φn(wsp_)) % post;       // (s_atom | ss_pre s) s_post*
  se_.as<φa_<π1>>() << s3 << s2;

  // IMPORTANT: we must prefer wse_ to wpa_; otherwise we will force [] to
  // always parse in plural context, which erases [s] as an s, which breaks
  // singular postfix operators against [] groups.
  let p1 = φa<π1>("πφp1",
                  wse_,
                  wpa_,
                  (wspp_ & wse_) % pre,
                  (wppp_ & wpe_) % pre);
  let p2 = (p1 & φn(wpp_)) % post;
  pe_.as<φa_<π1>>() << p2;

  def_sa(φgroup(wse_));
  def_pa(φgroup(wpe_));
  def_pa(wsa_);

  // Parens wrap a plural value into a single ηi
  def_sa(φ2("()", φlp_(), wpe_, φrp_())
         * [](π1 const &x)
           { return x | π0{"η", [](πi &i) { i.push(i.ypop()); }}; });

  def_t((φn(φ1("πpe`", wpe_, φl("`"))) & wpe_)
        % [](Vc<π1> &xs, π1 const &y)
          { π0 r;
            for (let &x : xs) r << (x | πf<-1>{"_", [](πi &i) { i.pop(); }});
            return r | y; });
}


}

#include "end.hh"

#endif
