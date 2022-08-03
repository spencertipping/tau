#ifndef tau_flux_ψ_h
#define tau_flux_ψ_h


#include <cassert>
#include <iostream>
#include <unordered_map>
#include <utility>


#include "../utf9.hh"

#include "init.hh"
#include "defs.hh"
#include "pipe.hh"
#include "lambda-defs.hh"
#include "lambda.hh"


#include "../module/begin.hh"

namespace tau::flux
{


struct Ψ
{
  Λ                          &c;
  std::unordered_map<ψi, ψ*>  ψs;
  std::unordered_map<ψi, λi>  block;

  ψi ψn;


  Ψ(Λ &c_) : c(c_), ψn(1) {}


  struct it
  {
    Ψ  *p;
    ψi  i;
    ψv  v;

    it()             : p(nullptr), i(0), v(u9n) {}
    it(Ψ &p_, ψi i_) : p(&p_), i(i_), v(u9n) {}

    ψv operator*() const { return v; }
    it &operator++()
      { if (!p->has_next(i)) p = nullptr, i = 0, v = u9n;
        else                 v = p->next(i);
        return *this; }

    bool operator==(it const &x) const
      { return p == x.p && i == x.i; }
  };

  struct ψit
  {
    Ψ  &p;
    ψi  i;
    it begin() const { return it(p, i); }
    it end()   const { return it(); }
  };


  // Returns pair<write_end, read_end>; these use different pipe IDs because
  // each end will be sent to a different Ψ
  std::pair<ψi, ψi> pipe(size_t capacity = 64);
  void unpipe(ψi, ψi);

  void close   (ψi);
  bool has_next(ψi);  // NOTE: has_next() -> next() must be λ-atomic
  ψv   next    (ψi);
  bool write   (ψi, ψv const &);
  ψit  all     (ψi i) { return ψit{*this, i}; }

  void unblock (ψi);

  static bool is_read_end(ψi i) { return !(i & 1); }
  static ψi   read_end   (ψi i) { return i & ~1ull; }
  static ψi   write_end  (ψi i) { return i |  1ull; }
};


std::pair<ψi, ψi> Ψ::pipe(size_t capacity)
{
  let p = new ψ(capacity);
  let r = ψn++ << 1;
  let w = r | 1ull;
  ψs[r] = p;
  ψs[w] = p;
  return std::make_pair(w, r);
}


void Ψ::unpipe(ψi w, ψi r)
{
  assert(r == read_end(w));
  assert(w == write_end(r));

  // TODO: map out the semantics here: what happens if a λ calls has_next()
  // repeatedly despite it returning false? Probably a user error.
}


void Ψ::close(ψi i)
{
  let p = ψs.at(i);

  // TODO: more precise semantic design
  if (!p->closed())
  {
    p->close();
    unblock(read_end(i));
    unblock(write_end(i));
  }
}


}

#include "../module/end.hh"


#endif
