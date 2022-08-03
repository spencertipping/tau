# Flux
The τ flux core (Γ) moves data through pipes (Ψ), scheduling coros (Λ) and managing time (Θ) in the process. Materially, Γ is a single UNIX process or WASM thread that uses a single address space, and typically runs an `epoll` or browser event loop.

Γ guarantees a unified, fully-ordered timeline for all λ running within it. Events across ζ/ψ within a single Γ are guaranteed to serialize deterministically, even across multiple λ. The flux IO boundary is also guaranteed to provide a unified view of the outside world.
