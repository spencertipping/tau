# π₁ intents
Types play a dual role in programming languages. First, they define how values are _stored;_ second, they encode a limited set of value semantics, e.g. OOP polymorphism and tagged unions. Because these considerations are coupled and runtime structure must be known, any type system targeting an untyped runtime must be _completeness-conservative:_ if a constraint can't be knowable in all cases, then the type system cannot attempt to solve for it. Type systems are further constrained by team-development expectations: inference must be stable across a variety of conditions.

π₁ has almost none of these issues. It's specialized for solo developers, UTF9 provides full RTTI, and its core behavior can change to suit specific examples and user preferences. Rather than defining the type of any given value, π₁ instead describes _intent:_ the developer's set of expectations about the value. This may or may not inform how the value is stored, and that representation need not be consistent over the scope of the intent.

Intents are much broader than types: `整数 → 整数` (`integer → integer`) is an intent, as is `x → x + 1`. These can be intersected to derive a unique function, although in many cases a function will be underconstrained and have multiple allowable implementations. Compiling a program involves finding any valid solution to the constraints that have been provided -- often optimizing the ambiguity for expected runtime performance. This is a mixture of solving pointwise constraints and constructing bridge connections between these points. Compilation is therefore EM across a sparse, discrete search graph.

This page is about how intents are defined, but see [π₁ spanning](pi1-spanning.md) for the beginning of the solver.


## Semantics
Semantic intents are ways to look at a value, not statements about the value's representation. For example, `人` ("person") is an intent that means something will be manipulated or viewed by a human with a user interface. `震级` (magnitude) and `角度` (angle) are ways to influence how such a value might be shown to a user, in this case by contextualizing it for them. So there might be an intent like `震级 ↔ 人` to allow a user to both observe and change a magnitude quantity. (Hopefully there's more to `震级` so the user knows what it's for. "Magnitude" isn't self-describing in most contexts.)

Some semantic intents have to do with runtime invariants the value is expected to possess. For example, `按x 排序` ("is ordered by `x`") might apply to a φ. Not all expectations are certain; `可能是按 x 排序` ("is probably ordered by `x`") allows π₁ to optimistically check for ordering and bail out to a sort process if it finds an exception. Without the "probably" indicator, π₁ would probably always sort; why would things be ordered without prior information? But that's not always efficient. Intents can be specified to an arbitrary degree; for example, "probably at 80%, and if unordered the exception will be in the first 5k values, and if unordered most subsequences of 100 values or fewer will still be ordered". No guarantee that downstream processes can use this information, but you can encode it anyway.


## Intent transformation
Every concrete (π₀) function transforms intent forwards: that is, it has no return-type polymorphism and hopefully not much argument polymorphism either.


## Encoding
We can derive intent encoding by looking at how we intend to structure our queries. It's user-optimal to provide many entropy-orthogonal query channels. From an ergonomic perspective, we also want queries to be self-describing; `x + 1` shouldn't require additional context. We don't need a `query → channel` function per se, but we should in general write intents such that unlabeled queries tend to match the correct one.

Functions transform intent separately for each channel, often ignoring channels when the function doesn't modify them.

**TODO:** how should this work -- should no-transform prevent the function from applying, is there a default, or do we just lose information (and presumably count this against our entropy) if we apply some functions?
