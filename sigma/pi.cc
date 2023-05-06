#include "pi.hh"
#include "begin.hh"

namespace σ
{

using namespace τ;


static T<φ<πf>, φ<πf>, φ<πfs>> φπ_() { slet r = πφ(φπa_(), φπm(), φπd(), φπt()); return r; }

φ<πf>  φπa()  { return φL<πf> ([]() { return std::get<0>(φπ_()); }); }
φ<πf>  φπs()  { return φL<πf> ([]() { return std::get<1>(φπ_()); }); }
φ<πfs> φπp()  { return φL<πfs>([]() { return std::get<2>(φπ_()); }); }

φ<πf>  φπa_() { return πφcore_a(); }
φ<πmf> φπm()  { return πφcore_m(); }
φ<πdf> φπd()  { return πφcore_d(); }
φ<πtf> φπt()  { return πφcore_t(); }


}

#include "end.hh"
