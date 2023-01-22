# Γ: τ compiler
Γ assembles multiple [γs](gamma.md) into a [τ](tau.md)-scale program.

Mathematically, Γ behaves just like γ but with different colocation semantics.

Structurally, Γ keeps track of resources assigned into each [π](pi.md) by keeping a set of π proxies, each of which contains a set of γs to be applied within that context (and therefore connected by [μs](muxi.md)). These π proxies are concatenatively connected by [ξs](muxi.md).

**TODO:** define this structure in more detail; we need to accommodate both linear and nonlinear routing.


## Nonlinear routing
It's possible to connect a γ to itself, or a later γ's output to an earlier γ's input. **These connections are always weak.** This and other types of nonlinear routing can be done with a Γ-hosted named channel -- basically a single-use variable.


## Building Γs
A Γ tracks γs with locality; it can be incrementally constructed by left or right-consing γs onto it.
