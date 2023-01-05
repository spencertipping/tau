# Tau
A Laplace transform of control flow, driven by an interpreted concatenative language.

+ [φ](doc/phi.md)
  + [Φ](doc/Phi.md)
  + [ζ](doc/zeta.md)
  + [Λ](doc/Lambda.md)
  + [UTF9](doc/utf9.md)
+ [π](doc/pi.md)
+ [γ](doc/gamma.md)
+ [Γ](doc/Gamma.md)

Production design:

+ [Runtime security](doc/security.md)


## Development setup
**Warning:** this repo requires a case-sensitive filesystem; it cannot be cloned correctly on case-insensitive MacOS or Windows.

On Ubuntu 22.04:

```sh
$ sudo dpkg --add-architecture i386
$ sudo apt update
$ sudo apt install -y docker.io  # for emsdk builds
$ sudo apt install -y \
       build-essential \
       libboost-fiber-dev libboost-context-dev \
       libmsgpack-dev \
       libxcb1-dev libx11-dev libx11-xcb-dev libgl-dev \
       libxcb-xinput-dev \
       g++-multilib-i686-linux-gnu \
       libc6-dbg{,:i386} \
       libstdc++6-12-dbg{,:i386} \
       libpango1.0-dev libcairo2-dev
```

**TODO:** dockerize native build environment


## Queue
+ Write orthogonal `πint` stack access methods
  + `πint::?(i9)` method to construct a pointer to large values
  + `<<` implementations should all return success/fail, or all return addresses (so φ `<<` and `πint <<` work similarly)
+ Make `i9::deref()` behavior consistent
+ `mmap` for π heaps on linux, so any violations are instant segfaults
+ Clean up `.dpush()` vs `<<`
  + Automatically refer to `i9`s that are already on the heap
+ GC completeness: fix indexed collections on write
+ GC performance: optimize data structures in markset, preallocate πho9
+ π performance: stack-allocate small values properly
+ Linux stdlib
  + Text line batching
  + JSONL batching
  + JSONRPC batching
  + Content-Length batching
  + External process stdio
+ WASM stdlib
+ π assembler macro definitions
