# ψ λ scope
ψ is the unit of process allocation within τ. More specifically, a ψ is referred to by any [ξs](xi.md) that it writes to; this jointly means that:

1. ψ GC follows dataflow dependency
2. ψs' _only_ form of communication is through ξs
3. All λs within a ψ are simultaneously deallocated when the ψ is destroyed

There are two types of ψs:

+ Basic ψs, which have no external interface
+ Server ψs, which behave like [Γs](Gamma.md) and allow connections

Server ports are named and each inbound [Ξ](Xi.md) can be annotated with an [η](eta.md) tag. This means there are a total of three arguments to a server-port connection:

```cpp
bool ψ::connect(St, η, Ξ);  // returns false to reject the connection
```

[π](pi.md) defines the following constructs to do this:

+ `!`: evaluate a Γ to a ψ
+ `:`: bind a ψ (or anything else) to a named variable
+ `&`: take a named ψ and return a Γ that connects to its server port
