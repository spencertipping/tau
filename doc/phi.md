# Φ
Φ is the smallest unit of atomicity we have; as a boundary, everything inside Φ is guaranteed to (1) operate on a single, totally-ordered timeline; (2) have process-persistent heap storage; and (3) be atomically instantiable by the flux core (Γ, via Λ).

Φ is also the target of the user-facing side of τ: programs are written as a flux-composed collection of Φ cells.

**TODO:** notation for referring to inside-Φ and outside-Φ (roughly, user-space and kernel-space)


## Instantiation semantics
