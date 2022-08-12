# Ψ
ζ-local IO is managed by Ψ, which tracks all internal connections and keeps λ states synchronized with ζ flow blocking.


## The server pattern
Open-ended components have the TCP server problem: we initiate a connection along a stable port, but must quickly change to a dedicated, dynamically-allocated port. In τ-world, just as in TCP-world, this should be done atomically -- that is, the handoff should happen with minimal context switching so we don't tie up the entry port for longer than is necessary.

The current architecture makes it impossible to use _zero_ context switches; client → server is _α_ + one message, and server → client is _α_ + one message to route + _ω_. Because the client yields after attempting to connect, we have a window of server downtime. This means we need to be able to monitor a port's availability with a wake queue (and to remain consistent, let's define a new λ state for this).

**TODO:** can we transparently redirect a connection? That would make the logic a lot simpler; then the client would just connect and be done with it.
