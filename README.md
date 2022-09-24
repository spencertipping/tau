# Tau
APL + Erlang + bash + Joy + FRP + probabilistic Prolog: a Laplace transform for control flow, where most code is data.

I will probably never have time to document this properly.

+ [φ](doc/phi.md) and [Φ](doc/Phi.md)
  + [ζ](doc/zeta.md)
  + [Λ](doc/Lambda.md)
  + [utf9](doc/utf9.md)
+ [γ](doc/gamma.md)
  + [γ design](doc/gamma-design.md)
  + [γ authoring](doc/gamma-authoring.md)
  + [γ topologies](doc/gamma-topologies.md)
+ [γ₀](doc/gamma0.md) and [Γ](doc/Gamma.md)
  + [Ξ](doc/Xi.md)
  + [ξ](doc/xi.md)
  + [π](doc/pi.md)
  + [∏](doc/Pi.md)
  + [η](doc/eta.md)
  + [security](doc/security.md)


## Development setup
On Ubuntu 22.04:

```sh
$ sudo dpkg --add-architecture i386
$ sudo apt update
$ sudo apt install -y docker.io  # for emsdk builds
$ sudo apt install -y \
       build-essential \
       libboost-fiber-dev libboost-context-dev libmsgpack-dev \
       libxcb1-dev libx11-dev libx11-xcb-dev libgl-dev \
       libxcb-xinput-dev \
       g++-multilib-i686-linux-gnu \
       libc6-dbg{,:i386} \
       libstdc++6-12-dbg{,:i386} \
       libpango1.0-dev libcairo2-dev
```

This repo cannot be cloned on case-insensitive filesystems.

**TODO:** dockerize build process


## Next steps
1. γ₀ → γ UTF9 API
2. [π](doc.pi.md)
3. Φ trunk multiplexing
4. γ₀ ED25519 + AES secure channels
5. Γ routing


### GL
+ Arc rendering with fragment shaders
+ Proper blending, also with shaders


### Data processing components
+ File reader with θ progress output
+ File writer
  + Frame format with boundary markers/SHAs
+ `zstd` compress/decompress
+ Shell command filter
+ JSON → UTF9 converter
+ Sort
+ APL-style operators


### Reactive components
+ Union
+ Gated register
