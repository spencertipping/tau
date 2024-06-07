# τ: programming in the s-domain
τ is a runtime system built around decaying steady states. If a bash command like `cat ... | grep ... | gzip > ...` defines a pipeline in a time-invariant way, then τ extends this by adding the ability to modify and extend the pipeline at runtime; that is, each part of the program is time-invariant but there are step changes. ([`ni`](https://github.com/spencertipping/ni) captures some of this, but ultimately falls short.)

Another way to think of it is that τ is to `ni` what the Laplace transform is to the Fourier transform: we can now describe systems that are locally time-invariant but which nonetheless change over time.

The τ runtime ships with the [σ standard library](doc/sigma.md).


## TODOs
+ Add OT Ψs to σ
+ WASM websocket client
+ HTTP websocket server
+ Formalize/test Λ scheduling system; right now there are probably some edge cases involving CPU blocking and Θ races

**Q:** what should OTs be able to synchronize? All η values? That would be pretty killer.


## Stray tests
```bash
$ bin/threaded-sfast
starting τ
starting worker
worker returning 42
worker returned 42
got 42
τ returned
all ok
```


## Compute model
```
+---------------------------------------+
| τ : Λ, epoll, ψ GC root               |
|                                       |
| +-----------+     +---+         +---+ |
| | ψ         |     | ψ |----ξ--->| ψ | |
| |           |--ξ->|   | ...     +---+ |
| | λ₁ λ₂ ... |     | λ |               |
| +-----------+     +---+               |
+---------------------------------------+
```

Note that ξs may point in arbitrary directions, i.e. some may point right-to-left and create graph cycles. Any cyclic ξs weakly refer to their sources, as ψ GC is driven by reference counting.

+ [τ](doc/tau.md) is the toplevel compute context, one per UNIX/WASM process
+ [Λ](doc/Lambda.md) is the λ threading manager
+ [λ](doc/lambda.md) is a single cooperative thread
+ [ψ](doc/psi.md) is an atomic λ scope
+ [ξ](doc/xi.md) is a bounded channel that carries η values
+ [η](doc/eta.md) is the data format used for ξ messages
+ [π](doc/pi.md): the expression language used to transform η values

τ implements an eager dataflow GC that deallocates ψs as soon as nobody depends on their ξ outputs. Destroying a ψ frees all of its global resources, including C++ data structures and any inbound ξs, which may in turn cause more ψs to be freed.

This τ GC corresponds to a signal exponentially decaying below the noise floor in the 𝓛 metaphor. At that point we quantize it to zero and it vanishes from computation.


## Language model
τ programs are written as a series of compositional dataflow equations that involve associative ξ-vector transformations called Γs.

+ [Ξ](doc/Xi.md): monomorphic, immutable cables of ξs
+ [Γ](doc/Gamma.md): polymorphic, immutable Ξ transformers
+ [Ψ](doc/Psi.md): monomorphic stream transformers
+ [φ](doc/phi.md): parser combinators

These are compiler objects, meaning that they don't actually contain live resources. Instead, Γs are functions that take Ξs as inputs and return Ξs as outputs, constructing any required native resources in the process.

Ξs have a relatively complex state space including a stack of ξ duplexes, variable bindings, and keyed sidecar ξs. Structurally, Ξ acts as an interpreter state for Γs, which are applied concatenatively.


## Included libraries
+ [`ctypes`](tau/ctypes.hh): aliases for C++ types, like `τ::St`
+ [`types`](tau/types.hh): `h256`, tuple accessors, time stuff
+ [Ω](doc/Omega.md): terminal storage (database) for [η](doc/eta.md) values


## Designing τ applications
+ [Data processing](doc/data-processing.md)
+ [Web applications](doc/web-applications.md)
+ [GL](doc/gl.md)


## Linux setup
```sh
$ ./build deps
$ make
```

**NOTE:** emscripten builds require a docker image that refers to internal infrastructure; if you can edit `dev/emsdk` to refer to the `emscripten/emsdk` image to use the stock image.
