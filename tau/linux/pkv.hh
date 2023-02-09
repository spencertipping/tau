#ifndef τlinux_pkv_h
#define τlinux_pkv_h


#include "../gamma.hh"
#include "../begin.hh"

namespace τ
{


// Retrieve elements from a k/v table
// key → (key, value)
Sp<γ> γpkv_get(St filename, St table);


// Insert/update elements into a k/v table
// (key, value) → key
Sp<γ> γpkv_set(St filename, St table);


}

#include "../end.hh"


#endif
