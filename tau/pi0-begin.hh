#include "debug.hh"
#include "begin.hh"


#define τπ0debug_enable_specialize_Gb_templates 1
#define τπ0debug_enable_specialize_O9_templates 0

// This exists to help LSP detect errors at edit-time by pre-specializing
// templates so LSP can do more analysis. It isn't enabled for full builds.
#if !defined(τπ0debug_specialize_templates)
# if !defined(τπ0_h)
#   define τπ0debug_specialize_templates 1
#   warning π₀ specialize_templates debug option is enabled (not production code)
# else
#   define τπ0debug_specialize_templates 0
# endif
#endif


#if τπ0debug_specialize_templates && τπ0debug_enable_specialize_Gb_templates
# define π0TG
# define π0TGn  template<>
# define π0TS   sletc Gb = 2;
# define π0T(t) t
#else
# define π0TG   template<τ::uN Gb>
# define π0TGn  π0TG
# define π0TS
# define π0T(t) t<Gb>
#endif

#if τπ0debug_specialize_templates && τπ0debug_enable_specialize_O9_templates
# define π0TO
# define π0To   o9vd
#else
# define π0TO   template<O9 T>
# define π0To   T
#endif


#define π0TGs  π0TG struct
#define π0R    π0T(π0r)
