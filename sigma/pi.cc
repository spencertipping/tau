#include "pi.hh"
#include "begin.hh"

namespace σ
{

using namespace τ;


static P<φ<πf>, φ<πf>> φπ_() { slet r = πφ(φπa_(), φπm(), φπd(), φπt()); return r; }

φ<πf>  φπa()  { return φL<πf>([]() { return φπ_().first; }); }
φ<πf>  φπ()   { return φL<πf>([]() { return φπ_().second; }); }

φ<πf>  φπa_() { return πφcore_a(); }
φ<πmf> φπm()  { return πφcore_m(); }
φ<πdf> φπd()  { return πφcore_d(); }
φ<πtf> φπt()  { return πφcore_t(); }


}

#include "end.hh"
