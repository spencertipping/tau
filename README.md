# τ: programming in 𝓛 space
τ is a runtime system built around temporary steady states. If a bash command like `cat ... | grep ... | gzip > ...` defines a pipeline in a time-invariant way, then τ extends this by adding the ability to modify and extend the pipeline at runtime; that is, each part of the program is time-invariant but there are step changes. ([`ni`](https://github.com/spencertipping/ni) captures some of this, but ultimately falls short.)

Another way to think of it is that τ is to `ni` what the Laplace transform is to the Fourier transform: we can now describe systems that are locally time-invariant but which nonetheless change over time.


## Compute model
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
+ [μ](doc/muxi.md) is a bounded channel that carries pointers to shared memory
+ [ξ](doc/muxi.md) is a bounded channel that carries serialized η values
+ [η](doc/eta.md) is the data format used for μ/ξ messages

τ implements an eager dataflow GC that deallocates ψs as soon as nobody depends on their μ or ξ output. Destroying a ψ frees all of its global resources, including C++ data structures and any other μ/ξ channels, which may in turn cause more ψs to be freed. If all ψs inside a π are destroyed, the π is also destroyed.

This τ GC corresponds to a signal exponentially decaying below the noise floor in the 𝓛 metaphor.


## Language model
