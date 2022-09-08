# γ authoring
How C++-hosted natives are written.


## Declarative elements
The γ initialization section of [Γ](Gamma.md) specifies a "pre-init" function that takes [Ξ](Xi.md) and creates any [φ](phi.md)s that might be mentioned within the [ξ](xi.md) connection list.

Because Ξ is fixed at construction-time, we should assume that any template-style unknowns have been resolved before [γ₀](gamma0.md) receives it. That is, γ doesn't participate in Ξ modification nor invariant propagation.


## γ lifecycle
Externally, γs are created by `γfs[Ξ[0]](Ξ[1])`; internally, that call does this:

1. Constructs a new γ
2. Calls the `pre_init` function registered for the γ variant, passing in the γ and `Ξ[1]`
3. Once all other γs have been initialized, connects the φs according to ξ
4. Calls the `init` function registered for the γ variant, passing in the γ and `Ξ[1]`; this creates the main λ for the γ
5. The γ's main λ is scheduled and, when it returns, destroys the γ


## Idioms
Many γs are just passthrough UTF9 transforms. These examine just one input at a time and maintain no state. We should have a way to write these with no loops, just a single UTF9→UTF9 C++ lambda. Any stream-state markers are passed through verbatim from input to output.
