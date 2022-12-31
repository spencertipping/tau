# γ
A [π₀](pi0.md)-hosted abstraction that contains λs and [φs](phi.md). The idea is to jointly define (1) polymorphic λ/φ containers, and (2) polysemantic connection operators. τ Laplacian eigenvectors are just π₀ functions that achieve these φ connections, so γ operators are eigenvector constituents.

Multiple γs can be combined to form [Γs](Gamma.md).

**TODO:** explain how this combination works; do we tuple up a bunch of piped-together γs first and then Γ-wrap that tuple? (Probably, but worth formalizing; we should take this up in [Γ](Gamma.md).)


## Data transformation
+ [`φ*<` and `φ*>`](gamma-phi-map.md)
+ [`φ»`](gamma-phi-parse.md)


## Encoding
A π₀ map with:

+ A set of λ pre/post filters (for `φ*`)
+ A set of boundary φs, each named
+ Once running, a set of λis
+ A finalizer function, or the default `[]`
+ A "control φ" that, when closed, will cause the γ to be destroyed

Note that γ destruction is a generic process that involves the finalizer function, `λ_`, and `φ_` in that order.

Each λ receives a mutable reference to the full γ map as its init argument.
