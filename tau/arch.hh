#ifndef τarch_h
#define τarch_h


#define τarch_linux64 1
#define τarch_linux32 2
#define τarch_wasm64  3
#define τarch_wasm32  4

#define τplatform_linux 1
#define τplatform_wasm  2

#define τcompiler_gcc   1
#define τcompiler_clang 2


#include <cstddef>
#include <cstdint>

#if INTPTR_MAX == INT32_MAX
# define τwordsize 32
#elif INTPTR_MAX == INT64_MAX
# define τwordsize 64
#else
# error invalid word size
#endif

static_assert(sizeof(void*)       << 3 == τwordsize);
static_assert(sizeof(std::size_t) << 3 == τwordsize);


#if defined(__clang__)
# define τcompiler τcompiler_clang
#elif defined(__GNUG__)
# define τcompiler τcompiler_gcc
#else
# warning tau builds have not been tested on this compiler, YMMV
#endif


#if defined(__linux__)
# define τhas_assert_fail      1
# define τhas_epoll            1
# define τhas_emscripten_fiber 0
# define τhas_zstd             1
# define τhas_fast_exceptions  1
# define τhas_fds              1
# define τplatform             τplatform_linux
# define τhas_threads          1
# define τhas_stacktrace       BOOST_STACKTRACE_USE_BACKTRACE

# if τwordsize == 64
#  define τarch              τarch_linux64
#  define τhas_boost_context 1
# elif defined(__LP32__)
#  define τarch              τarch_linux32
#  define τhas_boost_context 0
# endif

#elif defined(__EMSCRIPTEN__)
# define τhas_assert_fail      0
# define τhas_epoll            0
# define τhas_emscripten_fiber 1
# define τhas_zstd             0
# define τhas_boost_context    0
# define τhas_fast_exceptions  0
# define τhas_fds              0
# define τplatform             τplatform_wasm
# define τhas_threads          0  // FIXME?
# define τhas_stacktrace       0

# if τwordsize == 64
#  define τarch τarch_wasm64
# else
#  define τarch τarch_wasm32
# endif

#else
# error unsupported platform
#endif


#define τlinux      (τplatform == τplatform_linux)
#define τwasm       (τplatform == τplatform_wasm)
#define τemscripten τwasm

#define τgcc   (τcompiler == τcompiler_gcc)
#define τclang (τcompiler == τcompiler_clang)


#endif
