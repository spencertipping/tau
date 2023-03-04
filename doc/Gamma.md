# Γ: τ compiler
Γ assembles multiple [Γs](gamma.md) into a [τ](tau.md)-scale program. More specifically, Γ = 〈Γ〉.

Γ can be used as a Γ with `Γs`.


## Nonlinear routing
It's possible to connect a Γ to itself, or a later Γ's output to an earlier Γ's input. **These connections are always weak.** This and other types of nonlinear routing can be done with a Γ-hosted named channel -- basically a single-use variable.


## Splices and processors
Many Γ operations are simple _ξ → ξ_ transformations; that is, two-port unidirectional data filters. [Ξ](Xi.md) has many [ξ](xi.md) endpoints, the selection of which is orthogonal to the transformation being done to the data. So we create a factored group of Γ₂ defined as _(Ξ → (ξ, ξ), λ(ξ, ξ))_; that is, one function that splices the Ξ and one that processes the new pair in a [λ](lambda.md).

Γ₂ is not the only factoring. Entangled λs require more topologies, which include:

+ _Γ₄_ for a full bidirectional splice (and shared state)
+ _Γ«_ for left-singular, right-plural splicing (e.g. multiplex)
+ `Γ»_ for left-plural, right-singular splicing (e.g. demux)

**TODO:** more design here
