# π₀ GC contained regions
Object container relationships require us to analyze the pointer set before moving anything -- in particular, we need to avoid copying internal objects. We do this by sorting the set of live references and detecting internal ones; these will be rewritten but not independently copied.
