#include "../dep/picosha3.h"

#include "pi-fn.hh"
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


static void op_control();
static void op_bin();
static void op_debug();
static void op_map();
static void op_string();
static void op_tuple();


φ<πfn> φop()
{
  static bool i = false;
  if (!i)
  {
    φopd();
    i = true;

    op_control();
    op_bin();
    op_debug();
    op_map();
    op_string();
    op_tuple();
  }
  return φop_;
}


static void op_control()
{
  φopd().def(
    "⋄", binop(φπ(), "⋄", [](η a, η b) { return η1o(b); }));
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


static void op_map()
{
  φopd().def(
    "'", binop(φword(), "'", [](η a, η b) { return η1o(a[b.stv()]); }));
}


static void op_string()
{
  φopd().def(
    "j", φnull(πy<η0o>("j", [](η x) { return η1o(ηjson(x)); })),
    "J", φnull(πy<η0o>("J", [](η x) { return jsonη(x.stv()); })),
    "@", φnull(πy<η0o>("@", [](η x)
      { auto sha = picosha3::get_sha3_generator<256>();
        St r; r.resize(32);
        let v = x.stv();
        sha(v.begin(), v.end(), r.begin(), r.end());
        return η1o(r); })),
    "@@", φnull(πy<η0o>("@@", [](η x)
      { auto sha = picosha3::get_sha3_generator<256>();
        St r; r.reserve(64);
        Ar<u8, 32> h;
        let v = x.stv();
        sha(v.begin(), v.end(), h.begin(), h.end());
        for (uN i = 0; i < 32; ++i)
        { r.push_back("0123456789abcdef"[h[i] >> 4]);
          r.push_back("0123456789abcdef"[h[i] & 0x0f]); }
        return η1o(r); })));
}


static void op_tuple()
{
  φopd().def(
    "A", φnull(πy<η>("A", [](η x) { return x[0]; })),
    "B", φnull(πy<η>("B", [](η x) { return x[1]; })),
    "C", φnull(πy<η>("C", [](η x) { return x[2]; })),
    "D", φnull(πy<η>("D", [](η x) { return x[3]; })),
    "E", φnull(πy<η>("E", [](η x) { return x[4]; })),
    "F", φnull(πy<η>("F", [](η x) { return x[5]; })),
    "G", φnull(πy<η>("G", [](η x) { return x[6]; })),
    "H", φnull(πy<η>("H", [](η x) { return x[7]; })),
    "I", φnull(πy<η>("I", [](η x) { return x[8]; })));
}


}

#include "end.hh"
