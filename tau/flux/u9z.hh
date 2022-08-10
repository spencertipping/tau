#ifndef tau_flux_u9z_h
#define tau_flux_u9z_h


#include <variant>


#include "../types.hh"
#include "../utf9.hh"


#include "../module/begin.hh"


#define TAU_FLUX_U9_SHM 1

namespace tau::flux
{


#if TAU_FLUX_U9_SHM

  struct u9v
  {
    std::variant<SP<i9>, u9> const x;

    u9v()            : x{u9n} {}
    u9v(i9 *i)       : x{SP<i9>(i)} {}
    u9v(u9 &&u)      : x{u} {}
    u9v(u9 const &u) : x{u} {}

    // Force a copy to transfer ownership
    static u9v copy(u9 const &u)
      { o9 b; b << u;
        i9 *i = new i9;
        *i = b.convert_to_ibuf();
        return u9v(i); }

    // NOTE: ownership remains with u9v; to fix this, you need to
    // serialize out to a buffer using ::copy()
    operator u9() const { return x.index() == 0
                               ? u9(*std::get<SP<i9>>(x), 0)
                               : std::get<u9>(x); }
  };


  struct u9vs
  {
    uN operator()(u9v const &x) const
      { let s = x.x.index() == 0
              ? (*std::get<SP<i9>>(x.x)).size() + sizeof(i9)
              : std::get<u9>(x.x).bsize() + sizeof(u9);
        return sizeof(u9v) + s; }
  };

#else

  struct u9v
  {
    SP<i9> p;

    u9v(i9 *i) : p(i) {}
    u9v(u9 const &u)
      { o9 b; b << u;
        i9 *i = new i9();
        *i = b.convert_to_ibuf();
        p.reset(i); }

    static u9v copy(u9 const &u) { return u9v(u); }

    i9 operator*() const { return *p; }
    operator u9()  const { return u9(*p, 0); }
  };


  struct u9vs
  {
    uN operator()(u9v const &x) const
      { return sizeof(u9v) + sizeof(i9) + (*x).size(); }
  };

#endif


}

#include "../module/end.hh"


#endif
