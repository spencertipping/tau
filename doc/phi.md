# φ: γ and Γ assembly language
φ is a terse, `ni`-style language used to assemble [γs](gamma.md) and [Γs](Gamma.md). At its core, φ is a function that takes a string and returns a γ.

φ is built on longest-prefix matching with delegated parsing.


## φ structure
There are two base φs:

1. Γ: parse a γ name and delegate to the γ for the parse continuation
2. γ: parse the configuration and end, yielding back to Γ

Each takes _(x, i)_ and returns one of:

+ _(y, j)_: parse _x → y_ and resume at _j_
+ _(e, t, j)_: return error _e_ with backtrace _t_, at _j_

Errors are always reported at the rightmost point; that is, alt-folding and nesting will preserve the most-successful parse so we can report the most local problem to the user.
