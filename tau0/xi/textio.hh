#ifndef τξtextio_h
#define τξtextio_h


#include <cstring>


#include "begin.hh"


namespace τ::ξ
{


ϝ &split_chr(Φ &f, ch c)
{
  return *new ϝ(f, "split_chr", ϝ::ξι, [&, c](ϝ &f)
    { B s;
      for (let x : f)
      { if (x.type() != u9t::bytes) { f << x; continue; }
        let  z = x.size();
        u8c *l = x.data();
        if (!s.empty())
          if (let p = Rc<u8c*>(memchr(l, c, z - (l - x.data()))))
          { if (!(f << s + B{l, p + 1})) return;
            l = p + 1; }
        for (u8c *p;
             p = Rc<u8c*>(memchr(l, c, z - (l - x.data())));
             l = p + 1)
          if (!(f << Bv{l, p + 1})) return;
        s = B{l, Rc<u8c*>(x.data()) + z}; }
      if (!s.empty()) f << s; });
}


}

#include "end.hh"


#endif
