#include "debug.hh"
#include "begin.hh"


// This exists to help LSP detect errors at edit-time; by default it's disabled
#if !defined(τπ0debug_specialize_templates)

// Specialize templates when parsing with emacs/LSP; this #if
// will be skipped in production builds
# if !defined(τπ0_h)
#   define τπ0debug_specialize_templates 1
# else
#   define τπ0debug_specialize_templates 0
# endif

#endif


#if τπ0debug_specialize_templates
# define π0TG
# define π0TGn  template<>
# define π0TO
# define π0To   o9vd
# define π0TS   sletc Gb = 2;
# define π0T(t) t
# warning π₀ specialize_templates debug option is enabled (not production code)
#else
# define π0TG   template<τ::uN Gb>
# define π0TGn  π0TG
# define π0TO   template<O9 T>
# define π0To   T
# define π0TS
# define π0T(t) t<Gb>
#endif

#define π0TGs  π0TG struct
#define π0R    π0T(π0r)
