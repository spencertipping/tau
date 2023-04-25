#include "pi.hh"
#include "begin.hh"

namespace σ
{

using namespace τ;


φ<πf> &φπ()  { static φ<πf> r = πφ(φπa(), φπm(), φπd(), φπt()); return r; }

φ<πf>  φπa() { return πφcore_a(); }
φ<πmf> φπm() { return πφcore_m(); }
φ<πdf> φπd() { return πφcore_d(); }
φ<πtf> φπt() { return πφcore_t(); }


}

#include "end.hh"
