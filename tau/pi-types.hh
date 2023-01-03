#ifndef τπ0types_h
#define τπ0types_h


#include "types.hh"
#include "utf9.hh"

#include "begin.hh"

namespace τ
{


struct πabi;      // π₀ ABI (native FFI spec)
struct πpgm;      // π₀ program
struct πint;      // π₀ interpreter
struct πasm;      // π₀ assembler

struct πh;        // GC heap
struct πhs;       // GC heap space (one generation)
struct πhv;       // GC heap view (stack/frame/etc)
struct πms;       // GC markset
struct πho9;      // GC heap object copy (or copy-out-to-Φ)
struct πo9f;      // π₀ object export
struct πo9q;      // π₀ object quote-to-bytes


typedef u32 πbi;  // bytecode index
typedef u32 πfi;  // function index
typedef u32 πfa;  // function argument
typedef u64 πav;  // ABI version

typedef ζp  πr;   // reference to heap-allocated object


template<> struct o9_<πho9> { sletc v = true; };
template<> struct o9_<πo9f> { sletc v = true; };
template<> struct o9_<πo9q> { sletc v = true; };


// Bytecode function: takes a single 32-bit argument whose meaning
// is determined by the function
typedef void(*πf)(πint&, πfa);

// Bytecode instantiation: function index + arg
typedef P<πfi, πfa> πb;  static_assert(sizeof(πb) == 8);


inline auto πo9r(πr r)
{
  return o9flag{o9(u9_scoped<u9_π, ζp>{u9_π::ref, r})};
}


struct πsv  // stack view
{
  virtual ~πsv() {}
  virtual πsv         *up()     const = 0;
  virtual uN         size()     const = 0;
  virtual i9   operator[](uN i) const = 0;
  virtual void operator<<(πr x)       = 0;
  virtual void       drop(uN n = 1)   = 0;

  i9 pop() { let r = (*this)[0]; drop(1); return r; }
};


}

#include "end.hh"


#endif
