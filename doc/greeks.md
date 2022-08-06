# Greeks
Uppercase for outer views of ideas, lowercase for inner views. For example, an operator is either γ (the inner API) or Γ (the outer API, which manages many of them).

In code, suffixes modify concepts: `λi` is an ID for a λ, for example.

**NOTE:** these are τ-internal abstractions; for UTF9 state symbols, see [utf9.md](utf9.md).

+ γ: an operator, from the inside
  + ψ: γ-specific port
+ Γ: the flux core
  + Ψ: IO fabric
    + Φ: γ IO surface
    + ζ: unidirectional queue
  + Θ: time/deadline scheduler
  + Λ: execution multiplexer
    + λ: cooperative task
