# τ process
τ is the process boundary, which means that all side-effects happen by writing values to [ξs](xi.md) that it owns. Any [ψ](psi.md) writing to τ will be GC-live; as soon as the ψ has no direct or indirect write-path to τ it is deallocated, closing its input ξs.

τ provides named globals, which are used by some [Γs](Gamma.md) to create dynamic [Ξ](Xi.md) connections. These named globals are eigenvectors of the 𝓛 system.


## Forking
On platforms that provide a `fork()` syscall, τ provides its own `fork()` method that returns twice: once in the unmodified parent, and once in the detached child. When the child `fork()` returns, all file descriptors will have been removed from `epoll()` to prevent interference. Further, all ψs and λs are deallocated in the child, with whichever thread called `fork()` acting as main.
