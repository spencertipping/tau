# ψ λ scope
ψ is the unit of process allocation within τ. More specifically, a ψ is referred to by any [ξs](xi.md) that it writes to; this jointly means that:

1. ψ GC follows dataflow dependency
2. ψs' _only_ form of communication is through ξs
3. All λs within a ψ are simultaneously deallocated when the ψ is destroyed
