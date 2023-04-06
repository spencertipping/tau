# τ: programming in 𝓛 space
τ is a runtime system built around temporary steady states. If a bash command like `cat ... | grep ... | gzip > ...` defines a pipeline in a time-invariant way, then τ extends this by adding the ability to modify and extend the pipeline at runtime; that is, each part of the program is time-invariant but there are step changes. ([`ni`](https://github.com/spencertipping/ni) captures some of this, but ultimately falls short.)

Another way to think of it is that τ is to `ni` what the Laplace transform is to the Fourier transform: we can now describe systems that are locally time-invariant but which nonetheless change over time.

The τ runtime ships with the [σ standard library](doc/sigma.md).

**TODO:** GC the source and documentation for the sigma refactor


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
+ [π](doc/pi.md): the expression language used to transform η values

τ implements an eager dataflow GC that deallocates ψs as soon as nobody depends on their ξ outputs. Destroying a ψ frees all of its global resources, including C++ data structures and any inbound ξs, which may in turn cause more ψs to be freed.

This τ GC corresponds to a signal exponentially decaying below the noise floor in the 𝓛 metaphor.


## Language model
τ programs are written as a series of compositional dataflow equations that involve associative channel-vector transformations.

+ [Ξ](doc/Xi.md): monomorphic, immutable cables of ξs
+ [Γ](doc/Gamma.md): polymorphic, immutable Ξ transformers
+ [Ψ](doc/Psi.md): monomorphic stream transformers
+ [φ](doc/phi.md): parser combinators

These are compiler objects, meaning that they don't actually contain live resources. Instead, Γs are functions that take Ξs as inputs and return Ξs as outputs, constructing native resources in the process.

Ξs have a relatively complex state space including a stack of ξ duplexes, variable bindings, and keyed sidecar ξs. Structurally, Ξ acts as an interpreter state for Γs, which are applied concatenatively.


## Designing τ applications
+ [Data processing](doc/data-processing.md)
+ [Web applications](doc/web-applications.md)


## Scratch tests
```bash
$ bin/scratch
f(5) = 6
f("bar") = foobar
f(true) = fallthrough
a = 5
b = 5
string branch: foo
add1 got 7
add2 got 7
x + y = 7
1 || error = 1
0 || 3 = 3
```


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
