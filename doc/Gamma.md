# Γ: τ compiler
Γ assembles multiple [γs](gamma.md) into a [τ](tau.md)-scale program. More specifically, Γ = 〈γ〉.

Γ can be used as a γ with `γs`.


## Nonlinear routing
It's possible to connect a γ to itself, or a later γ's output to an earlier γ's input. **These connections are always weak.** This and other types of nonlinear routing can be done with a Γ-hosted named channel -- basically a single-use variable.
