# π₁
An intent-aware compiler that produces [π₀](pi0.md) bytecode. π₁ is multilevel in that it can consume direct bytecodes, APL-style imperative code, or high-level intent-driven implicit code (or a mixture of these).

π₁ views "code" as a search query over the space of programs, and that search query can have a varying amount of specificity.

π₁ interacts in realtime with [π₂](pi2.md), the source editor.

+ [π₁ code](pi1-code.md)
+ [π₁ intent](pi1-intent.md)
+ [π₁ spanning](pi1-spanning.md)
+ [π₁ modeling](pi1-modeling.md)


## Topological eigenvectors
`ni` vectors are implicitly chain-connected; no explicit syntax is required. We may be able to achieve some amount of this for π₁, but it isn't obvious. If we were to do that, we'd "carry" multiple φs around and bundle-connect them in some serial way. So ambient dependencies that we can grab implicitly; we declare the setup and chain the main dataflow.
