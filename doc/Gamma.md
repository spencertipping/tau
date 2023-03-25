# _Γ ∷ Ξ → Ξ_
Γs transform [Ξs](Xi.md), creating [ψs](psi.md) as processing elements. Γs can be combined associatively like matrices.

See also [Ψ](Psi.md), which is a more specific abstraction used to create ψs.

**NOTE:** some Γs can refer to global values and add connections. For example, we might have a shared database that can accept new RPC connections as it's running. It would internally use a multiplexer to track each input and reply with query results. Externally, that Γ would terminate a Ξ into a new multiplexed pair.
