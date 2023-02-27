# φ parser combinators
φ exists to support [π](pi.md), a terse, `ni`-style language used to assemble [Γs](gamma.md) and [Γs](Gamma.md). φ is a function that takes a string and returns a Γ.

φ is built on longest-prefix matching with delegated parsing, which enables parsers to be modified by extension libraries later on.


## φ structure
Each parser takes _(x, i)_ and returns one of:

+ _(y, j)_: parse _x → y_ and resume at _j_
+ _(e, t, j)_: return error _e_ with backtrace _t_, at _j_

Errors are always reported at the rightmost point; that is, alt-folding and nesting will preserve the most-successful parse so we can report the most local problem to the user.
