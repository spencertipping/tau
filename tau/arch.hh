#ifndef tau_arch_h
#define tau_arch_h


#define tau_arch_linux64 1
#define tau_arch_linux32 2
#define tau_arch_wasm64  3
#define tau_arch_wasm32  4

#define tau_platform_linux 1
#define tau_platform_wasm  2

#define tau_compiler_gcc 1
#define tau_compiler_clang 2


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


#if defined(__clang__)
# define tau_compiler tau_compiler_clang
#elif defined(__GNUG__)
# define tau_compiler tau_compiler_gcc
#else
# warning tau builds have not been tested on this compiler, YMMV
#endif


#if defined(__linux__)
# define tau_has_assert_fail 1
# define tau_has_epoll       1
# define tau_platform        tau_platform_linux

# if tau_wordsize == 64
#  define tau_arch              tau_arch_linux64
#  define tau_has_boost_context 1
# elif defined(__LP32__)
#  define tau_arch              tau_arch_linux32
#  define tau_has_boost_context 0
# endif

#elif defined(__EMSCRIPTEN__)
# define tau_has_assert_fail      0
# define tau_has_epoll            0
# define tau_platform             tau_platform_wasm
# define tau_has_emscripten_fiber 1

# if tau_wordsize == 64
#  define tau_arch tau_arch_wasm64
# else
#  define tau_arch tau_arch_wasm32
# endif

#else
# error unsupported platform
#endif


#endif
