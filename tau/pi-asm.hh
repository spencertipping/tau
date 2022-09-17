#ifndef τπasm_h
#define τπasm_h


#include "types.hh"
#include "utf9-types.hh"
#include "utf9.hh"

#include "pi-types.hh"
#include "pi-int.hh"
#include "pi-gc.hh"

#include "begin.hh"

namespace τ
{


struct πa
{
  static M<u9_symbol, πF*> fs;


  /*
   TODO: make something like this work, although this is probably
   the wrong way to think about it:

  template<class R, class... X>
  πF stackify(F<R(X...)> &&f)
    { return πF{sizeof...(X), [f = std::move(f)](πi &i, uN n) {
        FIXME: not gonna work
    }}; }
  */

  static void def(Stc &n, uN a, πf &f)
    { let s = u9_symbol::str(n);
      A(!fs.contains(s), "πa::def collision " << s);
      fs[s] = new πF{s, a, f}; }


};


}

#include "end.hh"


#endif
