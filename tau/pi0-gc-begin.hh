#include "debug.hh"
#include "begin.hh"


// This exists to help LSP detect errors at edit-time; by default it's disabled
#if !defined(τπ0debug_specialize_templates)
# define τπ0debug_specialize_templates 0
#endif


#if τπ0debug_specialize_templates
# define π0TG
# define π0TGn  template<>
# define π0TS   sletc Gb = 2;
# define π0T(t) t
# warning π₀ specialize_templates debug option is enabled (not production code)
#else
# define π0TG   template<τ::uN Gb>
# define π0TGn  π0TG
# define π0TS
# define π0T(t) t<Gb>
#endif
