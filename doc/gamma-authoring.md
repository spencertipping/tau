# γ authoring
How C++-hosted natives are written.


## γ₀
First, each τ program has a bootup γ, γ₀, that accepts further UTF9-encoded instructions to create γs and connect them. Each connection request consists of two pieces:

1. A map of `γid → (name config-map)` defining the γs we intend to create
2. A list of `(γid1, φ1, γid2, φ2)` tuples defining the space of ζs

Addresses are unique to a specific connection request. The γ₀ will reply with another mapping of `γid → (Γid = (Φid, Φγid))` that anchors each request-local γ ID to a permanent and globally unique one.
