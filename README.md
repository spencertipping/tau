# Tau
APL + Erlang + bash + Joy + Lisp + FRP + probabilistic Prolog: a Laplace transform for control flow, where most code is data.

Design notes:

+ [φ](doc/phi.md) and [Φ](doc/Phi.md)
  + [ζ](doc/zeta.md)
  + [Λ](doc/Lambda.md)
  + [UTF9](doc/utf9.md)
+ [γ](doc/gamma.md)
  + [γ design](doc/gamma-design.md)
  + [γ authoring](doc/gamma-authoring.md)
  + [γ topologies](doc/gamma-topologies.md)
+ [γ₀](doc/gamma0.md) and [Γ](doc/Gamma.md)
  + [Ξ](doc/Xi.md)
  + [ξ](doc/xi.md)
+ [π](doc/pi.md)
  + [π₀](doc/pi0.md)
  + [π₁](doc/pi1.md)
  + [π₂](doc/pi2.md)
  + [η](doc/eta.md)

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

**TODO:** dockerize native build process


## Queue
+ GC correctness: fix FIXME comments in `heap<<`
+ GC performance: optimize data structures in markset, preallocate π₀ho9
+ GC correctness test: make sure ζ values are unflagged, or we trace ζ buffers
+ Render arc with GL shader
