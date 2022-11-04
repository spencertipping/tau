#ifndef τξasm_h
#define τξasm_h


#include "begin.hh"

namespace τ::ξ
{


ϝ &utf9_asm(Φ &f)
{
  return *new ϝ(f, "utf9_asm", ϝ::ξι, [](ϝ &f)
    { B b; b.reserve(1l << ζb0);
      for (let x : f)
      { if (x.τ()) b.clear();
        if (!x) continue;
        b.append(Sc<Bv>(x));
        if (b.empty()) continue;
        uN i = 0;
        for (uN s = 0;
             i < b.size()
               && i + u9sb(u9ts_s(b[i])) < b.size()
               && i + (s = i9::size_of(b.data() + i)) <= b.size();
             i += s)
          if (!(f.β() << i9{b.data() + i}))
            break;
        b.erase(0, i); }});
}


ϝ &utf9_dasm(Φ &f, uN c = 0)
{
  // NOTE: c = 0 means zero-copy and no chunking
  return !c
    ? *new ϝ(f, "utf9_dasm", ϝ::ξι, [](ϝ &f)
      { for (let x : f) { if (!(f.β() << o9q{x})) break; }})

    : *new ϝ(f, "utf9_dasm", ϝ::ξι, [c](ϝ &f)
      { B b; b.reserve(1l << ζb0);
        { for (let x : f)
          { let o = o9(x);
            let s = o.size();
            let i = b.size();
            b.resize(i + s);
            if (let n = o.write(b.data() + i))
            { A(n != ζω, "utf9_dasm write failure");
              b.resize(b.size() + n - s); }
            if (b.size() >= c)
            { if (!(f.β() << b)) break;
              b.clear(); } }
          if (!b.empty()) f.β() << b; }});
}


}

#include "end.hh"


#endif
