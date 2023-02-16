#include "pi.hh"

#include "begin.hh"

namespace τ
{


static φ<πfn> φop_;

φd_<πfn> &φopd()
{ if (!φop_) φop_.reset(new φd_<πfn>);
  return *dpc<φd_<πfn>>(φop_); }


static φ<πfn> binop(φ<πfn> p, St name, F<η0o(η, η)> f)
{
  return φinsn(p, πinsn{name, [=](πi &i)
    { let r = i.dpop();
      let l = i.dpop();
      i.dpush(f(l.as_η(), r.as_η())); }});
}


static void op_bin();
static void op_debug();
static void op_tuple();


φ<πfn> φop()
{
  static bool i = false;
  if (!i)
  {
    φopd();
    i = true;

    op_bin();
    op_debug();
    op_tuple();
  }
  return φop_;
}


static void op_bin()
{
  φopd().def(
    "+", binop(φatom(), "+", [](η a, η b) { return η1o(a.pi() + b.pi()); }));
}


static void op_debug()
{
  φopd().def(
    "::I", φnull(πinsn("::I", [](πi &i) { std::cerr << i << std::endl; })),
    "::",  binop(φatom(), "::", [](η a, η b)
      { std::cerr << b.stv() << ": " << a << std::endl;
        return η1o(a); }));
}


static void op_tuple()
{
  φopd().def(
    "A", φnull(πy("A", [](η x) { return x[0]; })),
    "B", φnull(πy("B", [](η x) { return x[1]; })),
    "C", φnull(πy("C", [](η x) { return x[2]; })),
    "D", φnull(πy("D", [](η x) { return x[3]; })),
    "E", φnull(πy("E", [](η x) { return x[4]; })),
    "F", φnull(πy("F", [](η x) { return x[5]; })),
    "G", φnull(πy("G", [](η x) { return x[6]; })),
    "H", φnull(πy("H", [](η x) { return x[7]; })),
    "I", φnull(πy("I", [](η x) { return x[8]; })),
    "J", φnull(πy("J", [](η x) { return x[9]; })));
}


}

#include "end.hh"
