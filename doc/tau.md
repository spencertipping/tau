# τ process
τ is the process boundary, which means that all side-effects happen by writing values to [ξs](xi.md) that it owns. Any [ψ](psi.md) writing to τ will be GC-live; as soon as the ψ has no direct or indirect write-path to τ it is deallocated, closing its input ξs.

τ provides named globals, which are used by some [Γs](Gamma.md) to create dynamic [Ξ](Xi.md) connections. These named globals are eigenvectors of the 𝓛 system.
