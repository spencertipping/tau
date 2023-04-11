#ifndef τπφ_h
#define τπφ_h

#include "phi.hh"
#include "pi-val.hh"

#include "begin.hh"

namespace τ
{


// Constructs a full π grammar using the provided elemental parsers:
//
// a = atom
// m = monadic operator (just the operator)
// d = dyadic operator
// t = ternary operator
//
// The resulting parser performs all folding and returns a single value.

φ<πf> πφ(φ<πf> a, φ<πmf> m, φ<πdf> d, φ<πtf> t);


}

#include "end.hh"

#endif
