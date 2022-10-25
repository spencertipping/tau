#ifndef τπ0types_h
#define τπ0types_h


#include "types.hh"
#include "utf9.hh"

#include "begin.hh"

namespace τ
{


struct π0abi;      // π₀ ABI (native FFI spec)
struct π0pgm;      // π₀ program
struct π0int;      // π₀ interpreter
struct π0asm;      // π₀ assembler

struct π0h;        // GC heap
struct π0hs;       // GC heap space (one generation)
struct π0hv;       // GC heap view (stack/frame/etc)
struct π0ms;       // GC markset


typedef u32 π0bi;  // bytecode index
typedef u32 π0fi;  // function index
typedef u32 π0fa;  // function argument
typedef u64 π0av;  // ABI version

typedef ζp  π0r;   // reference to heap-allocated object


// Bytecode function: takes a single 32-bit argument whose meaning
// is determined by the function
typedef void(*π0f)(π0int&, π0fa);

// Bytecode instantiation: function index + arg
typedef P<π0fi, π0fa> π0b;  static_assert(sizeof(π0b) == 8);


inline auto π0o9r(π0r r)
{
  return o9flag{o9(u9_scoped<u9_π, ζp>{u9_π::ref, r})};
}


struct π0sv  // stack view
{
  virtual ~π0sv() {}
  virtual π0sv        *up()     const = 0;
  virtual uN         size()     const = 0;
  virtual π0r  operator[](uN i) const = 0;
  virtual void operator<<(π0r x)      = 0;
  virtual void       drop(uN n = 1)   = 0;

  π0r pop() { let r = (*this)[0]; drop(1); return r; }
};


}

#include "end.hh"


#endif
