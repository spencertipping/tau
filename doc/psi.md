# Ψ
ζ-local IO is managed by Ψ, which tracks all internal connections and keeps λ states synchronized with ζ flow blocking.


## The server pattern
Open-ended components have the TCP server problem: we initiate a connection along a stable port, but must quickly change to a dedicated, dynamically-allocated port. In τ-world, just as in TCP-world, this should be done atomically -- that is, the handoff should happen with minimal context switching so we don't tie up the entry port for longer than is necessary.

We can do this with `ψm`, which allows a γ to move a port from one `ψi` to another. So far so good, but in order to close the loop we also need to be able to `ψrw()` (or `ψφcw()` maybe) on a port that hasn't yet been connected.

**TODO:** define socket semantics precisely, in particular λ queueing
