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
template<class πφ> struct πφ_  // NOTE: CRTP for p(), see self()
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
  φ<π1> ts() const { return ws_; }
  φ<π1> tp() const { return wp_; }


#define def_defn(name, ptype) \
  Txs πφ &def_##name (Stc &n, Xs&&... xs) \
    { return def_<ptype>(name##_, n, std::forward<Xs>(xs)...); }

  def_defn(t,  π1)
  def_defn(s,  π1)
  def_defn(p,  π1)
  def_defn(pl, π1)
  def_defn(pr, π0)
  def_defn(sl, π1)
  def_defn(sr, π0)

#undef def_defn


  // NOTE: returning the wrong type is intentional. Every πsa<T> should
  // arise from a π1 that we parse, and it's up to πauto to convert the
  // π1 to T using immediate stack indirection. See pi-auto for details.
  Tt φ<πt <π1>> p(πt <T>*) const { return wt_  * [](π1 &&x) { return πt <π1>{mo(x)}; }; }
  Tt φ<πp <π1>> p(πp<T>*)  const { return wp_  * [](π1 &&x) { return πp <π1>{mo(x)}; }; }
  Tt φ<πs <π1>> p(πs<T>*)  const { return ws_  * [](π1 &&x) { return πs <π1>{mo(x)}; }; }
  Tt φ<πsl<π1>> p(πsl<T>*) const { return wsl_ * [](π1 &&x) { return πsl<π1>{mo(x)}; }; }
  Tt φ<πpl<π1>> p(πpl<T>*) const { return wpl_ * [](π1 &&x) { return πpl<π1>{mo(x)}; }; }

  // Not polymorphic because functions are always constant
  φ<πpr<π0>> p(πpr<π0>*) const { return wpr_ * [](π0 &&x) { return πpr<π0>{mo(x)}; }; }
  φ<πsr<π0>> p(πsr<π0>*) const { return wsr_ * [](π0 &&x) { return πsr<π0>{mo(x)}; }; }


protected:
  Tt struct is_φ       : std::false_type {};
  Tt struct is_φ<φ<T>> : std::true_type  {};

  Tx auto parsify(Stc &n, X &&x)
    { if constexpr (is_φ<De<X>>::value) return x;
      else                              return πauto(self(), n, std::function(std::forward<X>(x))); }

  Tt πφ &def_(φ<T>&, Stc&) { return self(); }

  template<class T, class X, class... Xs>  // T = parser type, X and Xs... are parsers
  πφ &def_(φ<T> &d, Stc &n, X const &f, Xs&&... xs)
    { auto &d_ = d.template as<φd_<T>>();
      if (!d_.has(n)) d_.def(n, φa0<T>(d.name() + n));
      let  p  = parsify(n, f);
      letc ok = std::is_same_v<De<decltype(p)>, φ<T>>;

      // NOTE: replace noisy << overload failure with a static_assert for brevity
      // in error reports
      static_assert(ok, "arity or parser-type mismatch in πφ::def_()");
      if constexpr (ok) d_.at(n).template as<φa_<T>>() << p;

      return def_(d, n, std::forward<Xs>(xs)...); }


  // All parsers are dispatch tables with φa<> per entry
  φ<π1> t_;   // toplevel program
  φ<π1> p_;   // plural full expression
  φ<π1> s_;   // singular full expression
  φ<π1> pl_;  // plural left
  φ<π0> pr_;  // plural right
  φ<π1> sl_;  // singular left
  φ<π0> sr_;  // singular left

  // Wrapped+weakened versions of the above, which allow whitespace
  φ<π1> wt_;  // wrap(φW(t_))
  φ<π1> wp_;
  φ<π1> ws_;
  φ<π1> wpl_;
  φ<π0> wpr_;
  φ<π1> wsl_;
  φ<π0> wsr_;
};


template<class πφ>
πφ_<πφ>::πφ_()
  : t_ (φd<π1>("π")),
    p_ (φd<π1>("πp")),
    s_ (φd<π1>("πs")),
    pl_(φd<π1>("πpl")),
    pr_(φd<π0>("πpr")),
    sl_(φd<π1>("πsl")),
    sr_(φd<π0>("πsr")),

    wt_ (φwrap(φW(t_))),
    wp_ (φwrap(φW(p_))),
    ws_ (φwrap(φW(s_))),
    wpl_(φwrap(φW(pl_))),
    wpr_(φwrap(φW(pr_))),
    wsl_(φwrap(φW(sl_))),
    wsr_(φwrap(φW(sr_)))
{
  // π ::= ... | (p '`')* p
  def_t("", (φn(φ1("πp`", wp_, φl("`"))) & wp_) % seq);

  // p ::= ((s ','?)+ | pl) pr* ';'?
  let sn = φn(ws_ << φo(φco_()), 1) * πφnp;  // (s ','?)+
  def_p("", ((sn | wpl_) & φn(wpr_)) % post << φo(φl(";")));

  // s ::= sl sr*
  def_s("", (wsl_ & φn(wsr_)) % post);

  // sl ::= '[' p ']' | '(' p ')'
  def_sl("[", φ1("[]", wp_, φrb_()));
  def_sl("(", φ1("()", wp_, φrp_()) * [](π1 const &x) { return x | πf_η(); });
}


}

#include "end.hh"

#endif
