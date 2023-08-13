#include "begin.hh"

namespace σ
{


// NOTE: each of these is defined in a separate .cc file to improve compilation times
void Γshared_ψ(Γφ&);

void πshared_atom(πφ&);
void πshared_int(πφ&);
void πshared_float(πφ&);
void πshared_η(πφ&);
void πshared_cmp(πφ&);
void πshared_list(πφ&);
void πshared_map(πφ&);
void πshared_string(πφ&);
void πshared_Γ(πφ&);


}

#include "end.hh"
