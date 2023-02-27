# Γ: τ compiler
Γ assembles multiple [Γs](gamma.md) into a [τ](tau.md)-scale program. More specifically, Γ = 〈Γ〉.

Γ can be used as a Γ with `Γs`.


## Nonlinear routing
It's possible to connect a Γ to itself, or a later Γ's output to an earlier Γ's input. **These connections are always weak.** This and other types of nonlinear routing can be done with a Γ-hosted named channel -- basically a single-use variable.
