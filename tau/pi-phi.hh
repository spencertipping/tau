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
  static π1 seq (Vc<π1> &xs, π1 const &y)
    { π0 r;
      for (let &x : xs) r << (x | πf_pop());
      return r | y; }


  φ<π1> t()  const { return wt_; }
  φ<π1> ts() const { return wse_; }
  φ<π1> tp() const { return wpe_; }

  Tt πφ &def_t (T const &f) { t_ .as<φa_<π1>>() << φauto(self(), f); return self(); }
  Tt πφ &def_sa(T const &f) { sa_.as<φa_<π1>>() << φauto(self(), f); return self(); }
  Tt πφ &def_se(T const &f) { se_.as<φa_<π1>>() << φauto(self(), f); return self(); }
  Tt πφ &def_pe(T const &f) { pe_.as<φa_<π1>>() << φauto(self(), f); return self(); }

  Txs πφ &def_ppre (Stc &n, Xs&&... xs) { return def_(ppre_,  n, std::forward<Xs>(xs)...); }
  Txs πφ &def_ppost(Stc &n, Xs&&... xs) { return def_(ppost_, n, std::forward<Xs>(xs)...); }
  Txs πφ &def_spre (Stc &n, Xs&&... xs) { return def_(spre_,  n, std::forward<Xs>(xs)...); }
  Txs πφ &def_spost(Stc &n, Xs&&... xs) { return def_(spost_, n, std::forward<Xs>(xs)...); }


  // NOTE: returning the wrong type is intentional. Every πsa<T> should
  // arise from a π1 that we parse, and it's up to πauto to convert the
  // π1 to T using immediate stack indirection. See pi-auto for details.
  Tt φ<πt <π1>> p(πt <T>*) const { return wt_  * [](π1 &&x) { return πt <π1>{mo(x)}; }; }
  Tt φ<πpe<π1>> p(πpe<T>*) const { return wpe_ * [](π1 &&x) { return πpe<π1>{mo(x)}; }; }
  Tt φ<πsa<π1>> p(πsa<T>*) const { return wsa_ * [](π1 &&x) { return πsa<π1>{mo(x)}; }; }
  Tt φ<πse<π1>> p(πse<T>*) const { return wse_ * [](π1 &&x) { return πse<π1>{mo(x)}; }; }


protected:
  πφ &def_(φ<π0>&, Stc&) { return self(); }

  Txxs πφ &def_(φ<π0> &d, Stc &n, X const &f, Xs&&... xs)
    { auto &d_ = d.as<φd_<π0>>();
      if (!d_.has(n)) d_.def(n, φa0<π0>(d.name() + n));
      d_.at(n).as<φa_<π0>>() << πauto(self(), n, std::function(f));
      return def_(d, n, std::forward<Xs>(xs)...); }


  φ<π1> t_;      // toplevel program (alt)
  φ<π1> pe_;     // plural expressions (alt)
  φ<π1> se_;     // singular expressions (alt)
  φ<π1> sa_;     // singular atoms (alt)
  φ<π0> ppre_;   // dsp
  φ<π0> ppost_;  // dsp
  φ<π0> spre_;   // dsp
  φ<π0> spost_;  // dsp

  // Wrapped+weakened versions of the above, which allow whitespace
  φ<π1> wt_;   // wrap(φW(t_))
  φ<π1> wpe_;
  φ<π1> wse_;
  φ<π1> wsa_;
  φ<π0> wppre_;
  φ<π0> wppost_;
  φ<π0> wspre_;
  φ<π0> wspost_;
};


template<class πφ>
πφ_<πφ>::πφ_()
  : t_    (φa0<π1>("π")),
    pe_   (φa0<π1>("πpe")),
    se_   (φa0<π1>("πse")),
    sa_   (φa0<π1>("πsa")),
    ppre_ (φd <π0>("πppre")),
    ppost_(φd <π0>("πppost")),
    spre_ (φd <π0>("πspre")),
    spost_(φd <π0>("πspost")),

    wt_    (φwrap(φW(t_))),
    wpe_   (φwrap(φW(pe_))),
    wse_   (φwrap(φW(se_))),
    wsa_   (φwrap(φW(sa_))),
    wppre_ (φwrap(φW(ppre_))),
    wppost_(φwrap(φW(ppost_))),
    wspre_ (φwrap(φW(spre_))),
    wspost_(φwrap(φW(spost_)))
{
  // π ::= ... | (p '`')* p
  def_t((φn(φ1("πpe`", wpe_, φl("`"))) & wpe_) % seq);

  // p ::= ((s ','?)+ | ppre p) ppost* ';'?
  let p1 = φn(wse_ << φo(φco_()), 1) * πφnp;  // (s ','?)+
  let p2 = (wppre_ & wpe_) % pre;             // ppre p
  def_pe(((p1 | p2) & φn(wppost_)) % post << φo(φl(";")));

  // s ::= (satom | spre s) spost*
  def_se(((wsa_ | (wspre_ & wse_) % pre) & φn(wspost_)) % post);

  // satom ::= '[' p ']' | '(' p ')'
  def_sa(φgroup(wpe_));
  def_sa(φ2("()", φlp_(), wpe_, φrp_()) * [](π1 const &x) { return x | πf_η(); });
}


}

#include "end.hh"

#endif
