#ifndef tau_arch_h
#define tau_arch_h


#define tau_arch_linux64 1
#define tau_arch_linux32 2
#define tau_arch_wasm64  3
#define tau_arch_wasm32  4


#include <cstdint>
#if INTPTR_MAX == INT32_MAX
# define tau_wordsize 32
#elif INTPTR_MAX == INT64_MAX
# define tau_wordsize 64
#else
# error invalid word size
#endif


#if defined(__linux__)
# if tau_wordsize == 64
#  define tau_arch tau_arch_linux64
# elif defined(__LP32__)
#  define tau_arch tau_arch_linux32
# endif
#elif defined(__EMSCRIPTEN__)
# if tau_wordsize == 64
#  define tau_arch tau_arch_wasm64
# else
#  define tau_arch tau_arch_wasm32
# endif
#endif


#endif
