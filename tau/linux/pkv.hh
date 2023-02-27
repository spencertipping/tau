#ifndef τlinux_pkv_h
#define τlinux_pkv_h


#include "../Gamma.hh"
#include "../begin.hh"

namespace τ
{


// Retrieve elements from a k/v table
// key → (key, value)
Γ Γpkv_get(St filename, St table);


// Insert/update elements into a k/v table
// (key, value) → key
Γ Γpkv_set(St filename, St table);


}

#include "../end.hh"


#endif
