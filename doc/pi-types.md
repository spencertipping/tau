# π types
π has a type system, but it's not principally about type _of values_. Instead, it's about variants of functions. Sometimes this involves specializing a function to a specific kind of value (e.g. `+` over vectors/tuples/strings), other times it can be in response to a constraint-as-test-case that reduces the space of acceptable implementations (e.g. to force a certain level of floating-point precision).

Everything about a function, from its name to its argument types, is a search term used to select an implementation and, in some cases, argument/return coercions.

π programs are generally underconstrained, which means the compiler can use discretion and choose function variants to optimize for some constraint: performance or debuggability, for instance.
