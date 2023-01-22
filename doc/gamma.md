# γ: ψ compiler
Mathematically, γ represents a [φ](phi.md)-space matrix of bytecode functions, each of which will be executed as a [λ](lambda.md).

Structurally, γ contains several things:

+ A map of `(k₁, k₂) → (f₁, f₂, ..., fn)` bindings
+ A map of `k₁ → k₂` left-transforms
+ A map of `k₁ → k₂` right-transforms

**NOTE:** bindings can have `k₁` and/or `k₂` set to `1` or `0`, respectively, to indicate that the resulting λ should be disconnected for reading or for writing.

**TODO:** add chan direction/weakness/π-locking metadata to the above

**TODO:** specify how γ can be applied (probably to ψ and φ)
