#ifndef tau_arch_h
#define tau_arch_h


#define tau_arch_linux64 1
#define tau_arch_linux32 2
#define tau_arch_wasm64  3
#define tau_arch_wasm32  4


#include <cstddef>
#include <cstdint>

#if INTPTR_MAX == INT32_MAX
# define tau_wordsize 32
#elif INTPTR_MAX == INT64_MAX
# define tau_wordsize 64
#else
# error invalid word size
#endif

static_assert(sizeof(void*)       << 3 == tau_wordsize);
static_assert(sizeof(std::size_t) << 3 == tau_wordsize);


#if defined(__linux__)
# define tau_has_assert_fail 1
# define tau_platform linux
# if tau_wordsize == 64
#  define tau_arch tau_arch_linux64
# elif defined(__LP32__)
#  define tau_arch tau_arch_linux32
# endif
#elif defined(__EMSCRIPTEN__)
# define tau_has_assert_fail 0
# define tau_platform wasm
# if tau_wordsize == 64
#  define tau_arch tau_arch_wasm64
# else
#  define tau_arch tau_arch_wasm32
# endif
#else
# error unsupported platform
#endif


#endif
