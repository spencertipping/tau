#ifndef τπ0types_h
#define τπ0types_h


#include "types.hh"
#include "utf9-types.hh"
#include "utf9.hh"

#include "begin.hh"

namespace τ
{


struct π0int;
struct π0asm;

typedef F<void(π0int&)> π0f;  // bytecode function
typedef uN              π0r;  // π₀ heap reference

constexpr uNc π0hω = -1;      // null reference


}

#include "end.hh"


#endif
