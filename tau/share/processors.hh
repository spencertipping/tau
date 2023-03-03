#ifndef τshare_processors_h
#define τshare_processors_h

#include "../eta1o.hh"
#include "../Gamma.hh"
#include "topology.hh"

#include "../begin.hh"

namespace τ
{


// Copy all elements from i to o, returning true if ξi was EOF and false
// if ξo terminated the output
bool ξcp(ξi, ξo);
inline bool ξcp (Sp<ψ>, ξi i, ξo o) { return ξcp(i, o); }
inline void ξcp_(Sp<ψ>, ξi i, ξo o) {        ξcp(i, o); }


template<class T> Γffn_ ξ1(T const &k) { return [k=η1o(k)](ξi i, ξo o) { ξcp(i, o);        o << k;  }; }
template<class T> Γffn_ ξK(T const &k) { return [k=η1o(k)](ξi i, ξo o) { ξcp(i, o); while (o << k); }; }


Γffn_ ξeach(F<void(η)>&&, bool tap = false);
Γffn_ ξmap (F<η0o(η)>&&);
Γffn_ ξτmap(F<η0o(η)>&&);


}

#include "../end.hh"

#endif
