# τ: programming in 𝓛 space
τ is a runtime system built around temporary steady states. If a bash command like `cat ... | grep ... | gzip > ...` defines a pipeline in a time-invariant way, then τ extends this by adding the ability to modify and extend the pipeline at runtime; that is, each part of the program is time-invariant but there are step changes. ([`ni`](https://github.com/spencertipping/ni) captures some of this, but ultimately falls short.)

Another way to think of it is that τ is to `ni` what the Laplace transform is to the Fourier transform: we can now describe systems that are locally time-invariant but which nonetheless change over time.

The τ runtime ships with the [σ standard library](doc/sigma.md).

**TODO:** GC the source and documentation for the sigma refactor

**TODO:** figure out how to do meta-linkage, e.g. Γ-level debug connections out to a monitor

**TODO:** move φ definitions into σ

**NOTE:** Ξ is fundamentally flawed as it stands. We need a virtual Ξ base with polymorphic `operator()(Γ2)` to cons on a new processor, plus `operator()(Γ4)` to do a full-duplex connection. These operators can change behavior for different cases, e.g. collecting a bundle.

**NOTE:** multiplex is one of many Ξ variants. We lazily collect Γs fed to it and it automatically connects them when new ξs are attached.


## Compute model
```
+-----------------------------------------+
| τ : Λ, epoll, ψ GC root                 |
|                                         |
| +-------------+     +---+         +---+ |
| | ψ : globals |     | ψ |----ξ--->| ψ | |
| |             |--ξ->|   | ...     +---+ |
| | λ₁ λ₂ ...   |     | λ |               |
| +-------------+     +---+               |
+-----------------------------------------+
```

+ [τ](doc/tau.md) is the toplevel compute context, one per UNIX/WASM process
+ [Λ](doc/Lambda.md) is the λ threading manager
+ [λ](doc/lambda.md) is a single cooperative thread
+ [ψ](doc/psi.md) is an atomic λ scope
+ [ξ](doc/xi.md) is a bounded channel that carries η values
+ [η](doc/eta.md) is the data format used for ξ messages

τ implements an eager dataflow GC that deallocates ψs as soon as nobody depends on their ξ outputs. Destroying a ψ frees all of its global resources, including C++ data structures and any inbound ξs, which may in turn cause more ψs to be freed.

This τ GC corresponds to a signal exponentially decaying below the noise floor in the 𝓛 metaphor.


## Language model
τ programs are written as a series of compositional dataflow equations that involve associative channel-vector transformations.

+ [Ξ](doc/Xi.md): a vector of ξ duplexes (really a map)
+ [Γ](doc/Gamma.md): matrices that transform Ξs and construct ψs
+ [φ](doc/phi.md): parser combinators
+ [π](doc/pi.md): the language to assemble τ programs

These are compiler objects, meaning that they don't actually contain live resources. Instead, Γs are functions that take Ξs as inputs and return Ξs as outputs, constructing native resources in the process.

See [compiler structures](doc/compiler-structures.md) for a quick rundown of how these structures interact.

See [Γ bindings](doc/gamma-bindings.md) and [π bindings](doc/pi-bindings.md) for a full description of the parser bindings used to construct Γs and π programs.


## Linux setup
```sh
$ sudo apt install -y \
       docker.io build-essential \
       libboost-context-dev libzstd-dev libsqlite3-dev
```

To get everything in `dev/hackery`, this might be a start (but no promises):

```sh
$ sudo dpkg --add-architecture i386
$ sudo apt install -y \
       build-essential \
       libboost-context-dev \
       libxcb1-dev libx11-dev libx11-xcb-dev libgl-dev \
       libxcb-xinput-dev \
       g++-multilib-i686-linux-gnu \
       libc6-dbg{,:i386} \
       libstdc++6-12-dbg{,:i386} \
       libpango1.0-dev libcairo2-dev \
       libzstd-dev libcurl4-openssl-dev
```
