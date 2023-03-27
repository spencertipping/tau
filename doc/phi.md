# φ parser combinators
General-purpose string parsers that can produce various types of values. [π](pi.md) uses φ to parse functions; there's a separate grammar for [Γ](Gamma.md) pipelines.

φ is built on longest-prefix matching with delegated parsing, which enables parsers to be modified by extension libraries later on.


## φ structure
Each parser takes _(x, i)_ and returns one of:

+ _(y, j)_: parse _x → y_ and resume at _j_
+ _(e, t, j)_: return error _e_ with backtrace _t_, at _j_

Errors are always reported at the rightmost point; that is, alt-folding and nesting will preserve the most-successful parse so we can report the most local problem to the user.
