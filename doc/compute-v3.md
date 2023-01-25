## Compute model (v3, TODO)
v3 adds shared memory in the form of π heaps, which are garbage-collected. I plan to replace v2 with this later on.

```
+-------------------------------------------------+
| τ : Λ, epoll, ψ GC root                         |
|                                                 |
| +-------------------------------+               |
| | π : πh                        |     +-------+ |
| |                               |     | π     | |
| | +-------------+     +---+     |     | +---+ | |
| | | ψ : globals |     | ψ |-----+--ξ--+-| ψ | | |
| | |             |--μ--|   | ... |     | +---+ | |
| | | λ₁ λ₂ ...   |     | λ |     |     +-------+ |
| | +-------------+     +---+     |               |
| +-------------------------------+               |
+-------------------------------------------------+
```

**NOTE:** all μ/ξ connections have ψ endpoints; this is important for τ GC, explained below.

+ [τ](doc/tau.md) is the toplevel compute context, one per UNIX/WASM process
+ [Λ](doc/Lambda.md) is the λ threading manager
+ [λ](doc/lambda.md) is a single cooperative thread
+ [π](doc/pi.md) is a GC'd heap
+ [ψ](doc/psi.md) is a global scope for λs
+ [ξ](doc/muxi.md) is a strong bounded channel that carries η values
+ [μ](doc/muxi.md) is a weak bounded channel that carries η values
+ [η](doc/eta.md) is the data format used for ξ/μ messages

τ implements an eager dataflow GC that deallocates ψs as soon as nobody depends on their μ or ξ output. Destroying a ψ frees all of its global resources, including C++ data structures and any other μ/ξ channels, which may in turn cause more ψs to be freed. If all ψs inside a π are destroyed, the π is also destroyed.

This τ GC corresponds to a signal exponentially decaying below the noise floor in the 𝓛 metaphor.
