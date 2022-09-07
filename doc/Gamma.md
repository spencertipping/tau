# Γ
It's possible to write operators just in terms of φ, λ, and ζ, but γ is preferable because it reorients those abstractions to focus on a single task. Similarly, it's possible to code distributed systems directly against Φ, but Γ ties it together by adding routing and RPCs. Γ is like a Φ that is aware of γ and provides distributed computing with no cognitive overhead by using multiplexed, encrypted trunk connections.


##  γ initialization
Handled by [γ₀](gamma0.md), the initial native within each Φ.

1. [Ξ](Xi.md) is consulted to determine what type of [γ](gamma.md) to create, and the [Φ](Phi.md) where it should be located
2. Γ routes the request to the target Φ, whose γ₀ receives the creation request
3. The new γ object runs its "pre-init" against Ξ, initializing all φ
4. Once all new γ have been initialized this way, connections are read from [ξ](xi.md) and ζs created to connect φs

This whole process is Φ-atomic; and because IDs are guaranteed to be unique, it is in practice Γ-atomic (up to RPC connection ordering).


## Φ trunks
Every Φ contains a unique private key, and accepts traffic encrypted with its public key.

γ₀ operates as a "trunk server", which allows encrypted φs from other Φs to demultiplex into Φ-internal traffic. Because γ₀ owns the private key for its Φ, only it can demultiplex foreign traffic into locally-routed messages. This is important for security, as γ₀ is also the component responsible for verifying that foreign messages are authorized to be sent to a given location.

As for the trunk connection itself, only a bidirectional channel is required. The two endpoint Φs negotiate a symmetric key with ED25519 and use that for further communication.

**TODO later:** implement the [Signal Protocol](https://en.wikipedia.org/wiki/Signal_Protocol) or another algorithm, e.g. [WireGuard](https://www.wireguard.com/protocol/).
