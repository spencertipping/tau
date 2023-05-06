#ifndef τπfn_η_h
#define τπfn_η_h


#include "pi-fn.hh"

#include "begin.hh"

namespace τ
{


πmsf πηflatten();

πtf πηcond();

πmf πηneg();
πmf πηinv();
πmf πηnot();
πmf πηnotnot();

πdf πηadd();
πdf πηsub();
πdf πηmul();
πdf πηdiv();
πdf πηmod();

πdf πηlsh();
πdf πηrsh();
πdf πηand();
πdf πηor();
πdf πηxor();

πdf πηlt();
πdf πηle();
πdf πηgt();
πdf πηge();
πdf πηeq();
πdf πηne();
πdf πηcmp();

πdf πηvlt();
πdf πηvle();
πdf πηvgt();
πdf πηvge();
πdf πηveq();
πdf πηvne();
πdf πηvcmp();

πdf πηpow();
πdf πηlogn();
πdf πηcopysign();
πdf πηhypot();

πmf πηexp();
πmf πηexp2();
πmf πηexpm1();
πmf πηlog();
πmf πηlog2();
πmf πηlog1p();

πmf πηsqrt();
πmf πηcbrt();

πmf πηsin();
πmf πηcos();
πmf πηtan();
πmf πηasin();
πmf πηacos();
πmf πηatan();
πdf πηatan2();

πmf πηsinh();
πmf πηcosh();
πmf πηtanh();
πmf πηasinh();
πmf πηacosh();
πmf πηatanh();

πmf πηerf();
πmf πηerfc();
πmf πηtgamma();
πmf πηlgamma();

πmf πηceil();
πmf πηfloor();
πmf πηtrunc();
πmf πηround();


}

#include "end.hh"


#endif
